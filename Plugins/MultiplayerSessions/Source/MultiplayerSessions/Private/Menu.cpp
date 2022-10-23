// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

void UMenu::MenuSetup()
{
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
