// Copyright Epic Games, Inc. All Rights Reserved.

#include "thrustGameMode.h"
#include "thrustCharacter.h"
#include "UObject/ConstructorHelpers.h"

AthrustGameMode::AthrustGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
