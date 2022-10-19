// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"

#include "MultiplayerShootGameCharacter.generated.h"

UCLASS(config = Game)
class AMultiplayerShootGameCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AMultiplayerShootGameCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	void OnResetVR();

	void MoveForward(float Value);

	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	//class TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> OnlineSessionPtr;
	IOnlineSessionPtr OnlineSessionPtr;

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	//创建游戏会话
	UFUNCTION(BlueprintCallable, Category = "Session")
		void CreateGameSession();

	//加入会话
	UFUNCTION(BlueprintCallable, Category = "Session")
		void JoinSession();

	//Steam 服务创建会话触发委托
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	//Steam Service 找到会话委托
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	//创建会话,回调函数
	UFUNCTION()
		void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	//查找会话回调函数
	UFUNCTION()
		void OnFindSessionsComplete(bool bWasSuccessful);
};

