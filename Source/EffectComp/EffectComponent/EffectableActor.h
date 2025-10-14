// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Util/EnumHelper.h"
#include "EffectableActor.generated.h"


enum class EStaticValueType : uint8;
class UMeshComponent;
struct FEntityReference;
struct FParamData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEffectableActor : public UInterface
{
	GENERATED_BODY()
};


#define EXECUTE_EFFECT(EnumType, Value) ExecuteEffectEvent<EnumType>(Value)
#define EXECUTE_EFFECT_ENTITY_ASYNC(EnumType, Value, ServerID) ExecuteEntityAcyncEffectEvent<EnumType>(Value, ServerID)

/**
 *
 */
class IEffectableActor
{
	friend struct FFXGroup;
	friend struct FEffectMaterialGroup;
	friend struct FEffectSoundGroup;
	GENERATED_BODY()
		typedef TBaseDynamicDelegate<FWeakObjectPtr, void, FString> FDelegate;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TArray<FName> GetEffectEventOptions() const PURE_VIRTUAL(IEffectableActor::GetEffectEventOptions, TArray<FName> Arr; return Arr;);

	template<typename E>
	void ExecuteEffectEvent(E EventEnum, FVector Offset = FVector::ZeroVector) const
	{
		FString EventString = EnumHelper::EnumToString<E>(EventEnum);
		ExecuteEffectEvent_Internal(EventString, Offset);
	}
	void ExecuteEffectEvent_Internal(FString EventName, FVector4 Offset) const;

	//프록시에서 호출하지 않도록 주의
	template<typename E>
	void ExecuteEntityAcyncEffectEvent(E EventEnum) const
	{
		FString EventString = EnumHelper::EnumToString<E>(EventEnum);
		ExecuteEntityAcyncEffectEvent_Internal(EventString);
	}

	void ExecuteEntityAcyncEffectEvent_Internal(FString EventName) const;
public:
};
