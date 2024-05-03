// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameInAMonthGameMode.generated.h"

class ATrainingDummy; // forward declaration to avoid circular dependency (error came up prior to adding this)
class AAI_Spawner; // forward declaration to avoid circular dependency (error came up prior to adding this)

UCLASS(minimalapi)
class AGameInAMonthGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameInAMonthGameMode();


	UFUNCTION(BlueprintImplementableEvent)
	void OnGameWin(); // event to be called when the player defeats ennemy

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameLose(); // event to be called when the player is defeated


	UFUNCTION(BlueprintCallable, Category = "Game")
	void GameWin();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void GameLose();


};



