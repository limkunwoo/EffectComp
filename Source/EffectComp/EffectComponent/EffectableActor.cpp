// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.


#include "EffectableActor.h"
#include "MVEffectComponent.h"
// Add default functionality here for any IEffectableActor functions that are not pure virtual.



void IEffectableActor::ExecuteEntityAcyncEffectEvent_Internal(FString EventName) const
{
	//const AActor* ThisActor = Cast<AActor>(this);
	//ThisActor->GetComponentByClass<UMVEffectComponent>()->ExecuteEntityAcyncEffectEvent(EventName);
}
void IEffectableActor::ExecuteEffectEvent_Internal(FString EventName, FVector4 Offset) const
{
	const AActor* ThisActor = Cast<AActor>(this);
	ThisActor->GetComponentByClass<UMVEffectComponent>()->ExecuteEffectEvent(EventName, Offset);
}


