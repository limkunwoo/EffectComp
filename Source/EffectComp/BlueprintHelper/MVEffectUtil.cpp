// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#include "MVEffectUtil.h"
#include "NiagaraFunctionLibrary.h"

namespace MVEffectUtil
{
	UNiagaraComponent* SpawnSystemAtLocation(const UObject* WorldContextObject, TSoftObjectPtr<UNiagaraSystem> SystemTemplate, FVector Location, FRotator Rotation, FVector Scale, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod, bool bPreCullCheck)
	{
		if (SystemTemplate.IsPending())
		{
			SystemTemplate = SystemTemplate.LoadSynchronous();
		}

		if (SystemTemplate.IsValid())
		{
			return UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemTemplate.Get(), Location, Rotation, Scale, bAutoDestroy, bAutoActivate, PoolingMethod, bPreCullCheck);
		}

		return nullptr;
	}

	UNiagaraComponent* SpawnSystemAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation, FVector Scale, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod, bool bPreCullCheck)
	{
		if (SystemTemplate)
		{
			return UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemTemplate, Location, Rotation, Scale, bAutoDestroy, bAutoActivate, PoolingMethod, bPreCullCheck);
		}

		return nullptr;
	}

	UNiagaraComponent* SpawnSystemAttached(TSoftObjectPtr<UNiagaraSystem> SystemTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod, bool bPreCullCheck)
	{
		if (SystemTemplate.IsPending())
		{
			SystemTemplate = SystemTemplate.LoadSynchronous();
		}

		if (SystemTemplate.IsValid())
		{
			return UNiagaraFunctionLibrary::SpawnSystemAttached(SystemTemplate.Get(), AttachToComponent, AttachPointName, Location, Rotation, LocationType, bAutoDestroy, bAutoActivate, PoolingMethod, bPreCullCheck);
		}

		return nullptr;
	}

	UNiagaraComponent* SpawnSystemAttached(UNiagaraSystem* SystemTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod, bool bPreCullCheck)
	{
		if (SystemTemplate)
		{
			return UNiagaraFunctionLibrary::SpawnSystemAttached(SystemTemplate, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bAutoDestroy, bAutoActivate, PoolingMethod, bPreCullCheck);
		}

		return nullptr;
	}
}
