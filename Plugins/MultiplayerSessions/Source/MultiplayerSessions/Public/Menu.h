// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	int32 NumPublicConections;
	FString MatchType;
	FString MapPath;

	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 parmNumPublicConections = 4, FString parmMatchType = "FreeForAll", FString parmMapPath = "/Game/Maps/Lobby");

protected:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

	//
	//Clicked Button CallbackFunction
	//
	UFUNCTION()
		void HostButtonClicked();
	UFUNCTION()
		void JoinButtonClicked();

	void MenuTearDown();

	//
	//MultiplayerSessionsSubsystem Delegate Callback Function
	//
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessed);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccessful);
	void OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
	UFUNCTION()
		void OnDestroySession(bool bSuccessful);
	UFUNCTION()
		void OnStartSession(bool bSuccessful);


public:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

protected:
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
};
