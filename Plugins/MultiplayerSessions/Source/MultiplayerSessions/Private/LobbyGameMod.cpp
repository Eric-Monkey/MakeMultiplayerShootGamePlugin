// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMod.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMod::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//统计玩家数量
	//在UE5，GameState被TShareUObject包裹 ,使用Get 获取 Value
	if (GameState)
	{
		int32 NumPlayer = GameState->PlayerArray.Num();

		//Debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				2,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Connected player numbers: %d"), NumPlayer)
			);
		}

		APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
		FString playerName = PS->GetPlayerName();

		//Debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				1,
				5.0f,
				FColor::Blue,
				FString::Printf(TEXT("%s has joined the game"), *playerName)
			);
		}
	}
}

void ALobbyGameMod::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (GameState) {
		APlayerState* PS = Exiting->GetPlayerState<APlayerState>();
		FString playerName = PS->GetPlayerName();
		//Debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				2,
				5.0f,
				FColor::Blue,
				FString::Printf(TEXT("%s has leaved the game"), *playerName)
			);
		}

		int32 NumPlayer = GameState->PlayerArray.Num();

		//Debug
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				1,
				30.0f,
				FColor::Blue,
				FString::Printf(TEXT("Connected player numbers: %d"), NumPlayer)
			);
		}
	}
}
