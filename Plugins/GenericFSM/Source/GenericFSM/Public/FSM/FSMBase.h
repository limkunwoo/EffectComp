// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSMBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GENERICFSM_API UFSMBase : public UObject
{
	GENERATED_BODY()
	
};

UCLASS(BlueprintType, MinimalAPI)
class UFSMBlueprint : public UBlueprint
{
	GENERATED_BODY()
	
};