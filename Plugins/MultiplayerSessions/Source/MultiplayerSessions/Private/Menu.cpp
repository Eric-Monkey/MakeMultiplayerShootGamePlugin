// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

bool UMenu::Initialize()
{

	if (!Super::Initialize()) {
		return false;
	}
	//Bind Delegate Function
	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}

	//
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	//Bind Callback function
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->OnCreateSession.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->OnFindSession.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->OnJoinSession.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->OnDestorySesssion.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->OnStartSession.AddDynamic(this, &ThisClass::OnStartSession);
	}

	return true;
}


void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::MenuSetup(int32 parmNumPublicConections, FString parmMathType, FString parnMapPath)
{
	//init Property
	NumPublicConections = parmNumPublicConections;
	MathType = parmMathType;
	MapPath = parnMapPath.Append("?listen");

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	APlayerController* PC = world->GetFirstPlayerController();
	if (PC) {
		FInputModeUIOnly InputModData;
		InputModData.SetWidgetToFocus(TakeWidget());
		InputModData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputModData);
		PC->SetShowMouseCursor(true);
	}
}

void UMenu::HostButtonClicked()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConections, MathType);
	}
}


void UMenu::JoinButtonClicked()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSession(10000);
	}
}

void UMenu::OnCreateSession(bool bWasSuccessed)
{
	if (bWasSuccessed) {
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(MapPath);
		}

		//debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Successed create session :NumPublicConections %d,MathType %s"), NumPublicConections, *MathType)
			);
		}
	}
	else
	{
		//debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				FString(TEXT("Fail create session :NumPublicConections %d,MathType %s"))
			);
		}
	}

}


void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccessfulbool)
{
	if (!MultiplayerSessionsSubsystem) {
		return;
	}

	if (bSuccessfulbool) {
		for (FOnlineSessionSearchResult SessionResult : SessionResults) {
			//参数匹配
			FString SettingVal;
			SessionResult.Session.SessionSettings.Get(FName("MathType"), SettingVal);

			if (SettingVal == MathType) {
				MultiplayerSessionsSubsystem->JoinSession(SessionResult);
				return;
			}
		}
	}


}

void UMenu::OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {

		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid()) {
			FString ConnectInfo;
			SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo);

			//加入游戏大厅
			APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
			if (PC) {
				PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bSuccessful)
{

}

void UMenu::OnStartSession(bool bSuccessful)
{

}


void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* PC = world->GetFirstPlayerController();
		if (PC) {
			FInputModeGameOnly InputModeGameOnly;
			PC->SetInputMode(InputModeGameOnly);
			PC->SetShowMouseCursor(false);
		}
	}
}



