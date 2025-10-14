// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EffectCompPawn.h"
#include "EffectCompSportsCar.generated.h"

class UMVEffectComponent;
/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class AEffectCompSportsCar : public AEffectCompPawn
{
	GENERATED_BODY()
	
public:

	AEffectCompSportsCar();

	UPROPERTY(EditAnywhere)
	UMVEffectComponent* EffectComponent;
};
