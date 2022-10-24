// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem) {
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConections, FString MatchType)
{

	if (!SessionInterface.IsValid()) {
		return;
	}

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {
		bIsDestory = false;
		//store var
		NumConnections = NumPublicConections;
		SessionSetVal = MatchType;
		DestroySession(NAME_GameSession);
		return;
	}

	////添加委托到为委托列表 并 保存委托句柄
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	//SessionSetting
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = /*IOnlineSubsystem::Get()->GetSubsystemName() == "Null" ? true : */false;
	LastSessionSettings->NumPublicConnections = NumPublicConections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//Create Session
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings)) {
		//从委托列表移除
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		OnCreateSession.Broadcast(false);
		return;
	}
	//debug
	/*if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Yellow,
			FString::Printf(TEXT("HostButtonClicked to call create session :NumPublicConections %d,MathType %s"), NumPublicConections, *MathType)
		);
	}*/

}

void UMultiplayerSessionsSubsystem::FindSession(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) {
		return;
	}
	//添加委托列表
	SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	LastSessionSearch->bIsLanQuery = /*IOnlineSubsystem::Get()->GetSubsystemName() == "Null" ? true : */false;
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);


	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef())) {
		//委托列表移除
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		//广播
		OnFindSession.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	//debug
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString(TEXT("Enter MultiplayerSessionSubsystem JoinSession"))
		);
	}

	if (!SessionInterface.IsValid()) {
		OnJoinSession.Broadcast(FName(), EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	//返回委托句柄
	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult)) {
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		OnJoinSession.Broadcast(FName(), EOnJoinSessionCompleteResult::UnknownError);
	}

}

void UMultiplayerSessionsSubsystem::DestroySession(FName SessionName)
{
	if (!SessionInterface.IsValid()) {
		OnDestorySesssion.Broadcast(false);
		return;
	}
	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	//Failed Destroy
	if (! SessionInterface->DestroySession(SessionName)) {
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		OnDestorySesssion.Broadcast(false);
	}

}

void UMultiplayerSessionsSubsystem::StartSession()
{

}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool isWasSuccessful)
{
	if (SessionInterface) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	OnCreateSession.Broadcast(isWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface) {
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	TArray<FOnlineSessionSearchResult> SessionSearchResults = LastSessionSearch->SearchResults;
	if (SessionSearchResults.Num() <= 0) {
		//广播
		OnFindSession.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	OnFindSession.Broadcast(SessionSearchResults, bWasSuccessful);

}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid()) {
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}

	//debug
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString(TEXT("Enter MultiplayerSessionSubsystem OnJoinSession"))
		);
	}

	OnJoinSession.Broadcast(SessionName, Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool isWasSuccessful)
{
	if (SessionInterface.IsValid()) {
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	
	if (! bIsDestory && isWasSuccessful) {
		bIsDestory = true;
		CreateSession(NumConnections,SessionSetVal);
	}

	OnDestorySesssion.Broadcast(isWasSuccessful);
	
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool isWasSuccessful)
{

}
