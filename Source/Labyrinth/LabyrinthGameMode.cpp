// Copyright Epic Games, Inc. All Rights Reserved.

#include "LabyrinthGameMode.h"
#include "LabyrinthCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALabyrinthGameMode::ALabyrinthGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
