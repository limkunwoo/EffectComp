// Copyright Epic Games, Inc. All Rights Reserved.

#include "EffectCompWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UEffectCompWheelFront::UEffectCompWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}