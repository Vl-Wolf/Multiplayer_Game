// Copyright Epic Games, Inc. All Rights Reserved.

#include "TTGameMode.h"
#include "TTCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATTGameMode::ATTGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
