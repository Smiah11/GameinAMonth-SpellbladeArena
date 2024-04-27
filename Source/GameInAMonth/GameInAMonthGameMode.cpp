// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameInAMonthGameMode.h"
#include "GameInAMonthCharacter.h"
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
