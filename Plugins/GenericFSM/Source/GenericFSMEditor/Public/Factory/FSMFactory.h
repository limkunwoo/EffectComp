// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "Script/AssetDefinition_Blueprint.h"
#include "Factories/BlueprintFactory.h"
#include "FSMFactory.generated.h"

/**
 * 
 */
UCLASS()
class GENERICFSMEDITOR_API UFSMFactory : public UBlueprintFactory
{
	GENERATED_BODY()
    UFSMFactory(const FObjectInitializer& ObjectInitializer);
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    virtual bool ShouldShowInNewMenu() const override;
};

UCLASS()
class UAssetDefinition_FSMBlueprint : public UAssetDefinition_Blueprint
{
	GENERATED_BODY()
public:
	virtual FText GetAssetDisplayName() const override { return FText::FromString(TEXT("Finite State Machine")); }
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
	{
		static const auto Categories = { EAssetCategoryPaths::Basic, FAssetCategoryPath(FText::FromString(TEXT("StateMachine")))};
		return Categories;
	}
	virtual FLinearColor GetAssetColor() const override { return FLinearColor::Yellow; }

	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
};
