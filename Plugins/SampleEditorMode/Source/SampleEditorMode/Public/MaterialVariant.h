// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MaterialVariant.generated.h"

/**
 * 
 */
UENUM()
enum class EMateriaVariantIndex
{
	Bool,
	Int64,
	String,
	LinearColor,
	Max
};

USTRUCT(BlueprintType)
struct SAMPLEEDITORMODE_API FMaterialVariant
{
	GENERATED_BODY()
public:
	using VariantType = TVariant<bool, int64, FString, FLinearColor>;
	using EVariantIndex = EMateriaVariantIndex;
	static_assert(TVariantSize_V<VariantType> == (int32)EVariantIndex::Max, "");
public:
	FMaterialVariant();

	template<typename T>
	T TryGet()
	{
		return Variant.TryGet<T>();
	}
	template<typename T>
	void Set(T&& Value)
	{
		Variant.Set<std::remove_cvref_t<T>>(Forward<T>(Value));
		Index = (EVariantIndex)Variant.GetIndex();
	}

	template<typename T>
	FMaterialVariant& operator=(T&& InValue)
	{
		Set<T>(Forward<T>(InValue));
	}

	template<typename Func>
	decltype(auto) Visit(Func&& Callable)
	{
		return ::Visit(Forward<Func>(Callable), Variant);
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << Variant;
		return true;
	}

private:
	template<typename T>
	friend class FVariantDetailCustomization;
	VariantType Variant;
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	EMateriaVariantIndex Index;
#endif
};

template<> struct TStructOpsTypeTraits<FMaterialVariant> : public TStructOpsTypeTraitsBase2<FMaterialVariant>
{
	enum
	{
		WithSerializer = true,
	};
};