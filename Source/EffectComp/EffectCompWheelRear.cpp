// Copyright Epic Games, Inc. All Rights Reserved.

#include "EffectCompWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UEffectCompWheelRear::UEffectCompWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}