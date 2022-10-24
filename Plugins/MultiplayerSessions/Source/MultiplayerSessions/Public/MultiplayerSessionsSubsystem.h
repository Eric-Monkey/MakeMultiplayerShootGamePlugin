// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

//D : \UE\UE_4.26\

#include "MultiplayerSessionsSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerCreateSessionComplete, bool, bWasSuccessced);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerJoinSessionComplete, FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerDestorySessionComplete, bool, bSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerStartSessionComplete, bool, bSuccessful);


/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UMultiplayerSessionsSubsystem();

	//
	// handle to Session Function called by Menu
	//
	void CreateSession(int32 NumPublicConections,FString MatchType);
	void FindSession(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession(FName SessionName);
	void StartSession();

	//
	// Custom Delegate
	//
	FMultiplayerCreateSessionComplete OnCreateSession;
	FMultiplayerFindSessionComplete OnFindSession;
	FMultiplayerJoinSessionComplete	OnJoinSession;
	FMultiplayerDestorySessionComplete OnDestorySesssion;
	FMultiplayerStartSessionComplete  OnStartSession;

protected:
	//
	//Internal CallBack Function,this don't need to call outside this class.
	//
	void OnCreateSessionComplete(FName SessionName, bool isWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool isWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool isWasSuccessful);


private:
	bool bIsDestory;
	
	//CreateSession Need
	int32 NumConnections;
	FString SessionSetVal;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch = MakeShareable(new FOnlineSessionSearch());

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	//
	// will Add Session Interface Delegate List
	// 
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

};
