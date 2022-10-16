// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerShootGameGameMode.h"
#include "MultiplayerShootGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerShootGameGameMode::AMultiplayerShootGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
