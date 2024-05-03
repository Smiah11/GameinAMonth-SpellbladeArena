// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameInAMonthGameMode.h"
#include "GameInAMonthCharacter.h"
#include "TrainingDummy.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGameInAMonthGameMode::AGameInAMonthGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGameInAMonthGameMode::GameWin()
{



	// check if the enemy is dead and reset the player  - this function will be called in the spawner
	AGameInAMonthCharacter* Player = Cast<AGameInAMonthCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->ResetPlayer();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	OnGameWin();


}

void AGameInAMonthGameMode::GameLose()
{


	AGameInAMonthCharacter* Player = Cast<AGameInAMonthCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player->bIsDead)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		OnGameLose();
	}
	else
	{
		return;
	}
}
