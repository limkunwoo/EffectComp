// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "NiagaraComponentPoolMethodEnum.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USceneComponent;

// NiagaraFunctionLibrary.h가 무겁기 때문에 최대한 사용을 줄이기 위한 헤더파일
namespace MVEffectUtil
{
	UNiagaraComponent* SpawnSystemAtLocation(const UObject* WorldContextObject, TSoftObjectPtr<UNiagaraSystem> SystemTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = static_cast<ENCPoolMethod>(0), bool bPreCullCheck = true);
	UNiagaraComponent* SpawnSystemAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = static_cast<ENCPoolMethod>(0), bool bPreCullCheck = true);
	UNiagaraComponent* SpawnSystemAttached(TSoftObjectPtr<UNiagaraSystem> SystemTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = static_cast<ENCPoolMethod>(0), bool bPreCullCheck = true);
	UNiagaraComponent* SpawnSystemAttached(UNiagaraSystem* SystemTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = static_cast<ENCPoolMethod>(0), bool bPreCullCheck = true);
}
