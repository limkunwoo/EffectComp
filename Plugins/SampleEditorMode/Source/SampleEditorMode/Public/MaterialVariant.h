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
	Texture,
	Max
};

class UTexture;

template<typename TVariant, typename TDerived>
struct PLATFORM_EMPTY_BASES TUStructVariantImpl
{
	using VariantType = TVariant;
	using ImplType = TUStructVariantImpl<TVariant, TDerived>;

	template<typename T>
	T* TryGet()
	{
		return Variant.TryGet<T>();
	}
	template<typename T>
	void Set(T&& Value)
	{
		Variant.Set<std::remove_cvref_t<T>>(Forward<T>(Value));
	}

	template<typename T>
	TUStructVariantImpl& operator=(T&& InValue)
	{
		Set<T>(Forward<T>(InValue));
	}
	
	bool Serialize(FArchive& Ar)
	{
		Ar << Variant;
		return true;
	}
	
	template<typename Func>
	decltype(auto) Visit(Func&& Callable)
	{
		return ::Visit(Forward<Func>(Callable), Variant);
	}

	int32 GetIndex() const
	{
		return Variant.GetIndex();
	}
	VariantType Variant;
};

USTRUCT(BlueprintType)
struct SAMPLEEDITORMODE_API FVariantBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct SAMPLEEDITORMODE_API FMaterialVariant : public FVariantBase, public TUStructVariantImpl<TVariant<bool, double, FString, FLinearColor, TSoftObjectPtr<UTexture>>, FMaterialVariant>
{
	GENERATED_BODY()
public:
	using ImplType::operator=;
};


template<> struct TStructOpsTypeTraits<FMaterialVariant> : public TStructOpsTypeTraitsBase2<FMaterialVariant>
{
	enum
	{
		WithSerializer = true,
	};
};