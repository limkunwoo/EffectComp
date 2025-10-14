// Copyright Epic Games, Inc. All Rights Reserved.

#include "EffectCompGameMode.h"
#include "EffectCompPlayerController.h"

AEffectCompGameMode::AEffectCompGameMode()
{
	PlayerControllerClass = AEffectCompPlayerController::StaticClass();
}
