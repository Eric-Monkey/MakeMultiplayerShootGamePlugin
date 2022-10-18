// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerShootGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "Templates/SharedPointer.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayerShootGameCharacter

AMultiplayerShootGameCharacter::AMultiplayerShootGameCharacter():
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMultiplayerShootGameCharacter::OnCreateSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	//SubSystem
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
	
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::White,
				FString::Printf(TEXT("Found System: %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerShootGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayerShootGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayerShootGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayerShootGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayerShootGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayerShootGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayerShootGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiplayerShootGameCharacter::OnResetVR);
}


void AMultiplayerShootGameCharacter::OnResetVR()
{
	// If MultiplayerShootGame is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MultiplayerShootGame.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiplayerShootGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMultiplayerShootGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMultiplayerShootGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShootGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShootGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayerShootGameCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayerShootGameCharacter::CreateGameSession()
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	FNamedOnlineSession*  ExistingSession =  OnlineSessionPtr->GetNamedSession(NAME_GameSession);
	
	//�Ự��������
	if (ExistingSession) {
		OnlineSessionPtr->DestroySession(NAME_GameSession);
	}
	
	////���ί�е�Ϊί���б�
	OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);


	//���ô����Ự
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettings = MakeShareable(new FOnlineSessionSettings());
	OnlineSessionSettings->bIsLANMatch = false;		//�����������ر�
	OnlineSessionSettings->NumPublicConnections =4;	//	Connect number
	OnlineSessionSettings->bAllowJoinInProgress = true; //Session Runtime Can join?
	OnlineSessionSettings->bAllowJoinViaPresence = true;	//region
	OnlineSessionSettings->bShouldAdvertise = true; //service board
	OnlineSessionSettings->bUsesPresence = true; // support for find
		 

	//�Ự�����ڣ������»Ự
	const ULocalPlayer*  LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionPtr->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *OnlineSessionSettings);
}

void AMultiplayerShootGameCharacter::OnCreateSessionComplete(FName SessionName, bool isCreated)
{
	if (isCreated) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Created sesssion Name: %s"), *SessionName.ToString())
			);
		}
	}
	else
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString(TEXT("Failed to create session !"))
			);
		}
	}
}

