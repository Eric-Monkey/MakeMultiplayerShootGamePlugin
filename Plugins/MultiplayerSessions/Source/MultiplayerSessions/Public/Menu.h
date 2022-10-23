// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"


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
	FString MathType;
	FString MapPath;

	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 parmNumPublicConections = 4, FString parmMathType = "FreeForAll", FString parmMapPath = "/Game/Maps/Lobby");

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

public:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

protected:
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
};
