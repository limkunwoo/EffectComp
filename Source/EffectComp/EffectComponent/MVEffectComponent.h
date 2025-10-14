// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Util/EnumHelper.h"
#include "MVEffectGroup.h"
#include "MVEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEffectEvent, FString, Condiotion, FVector, Offset);
class IEffectableActor;
UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class UMVEffectComponent : public UActorComponent
{
	friend struct FFXGroup;
	friend struct FEffectMaterialGroup;
	friend struct FEffectSoundGroup;
	friend struct FEffectGroup;
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMVEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//~Begin UObjectInterface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ActivateEffectGroup(EEffectType EffectType, FName GroupName, bool bNewActive);
	UFUNCTION(BlueprintCallable)
	void SetParameter(EEffectType EffectType, FName GroupName, FParamData Param);
	
	UFUNCTION(CallInEditor)
	const TArray<FName> GetActiveOptions() const;
	
	UFUNCTION(CallInEditor)
	const TArray<FName> GetAllMesh() const
	{
		AActor* Owner = GetOwner();
		TArray<UMeshComponent*> ChildMeshs;
		Owner->GetComponents<UMeshComponent>(ChildMeshs);
		TArray<FName> MeshNameArr;

		for (UMeshComponent* MeshComp : ChildMeshs)
		{
			MeshNameArr.Add(MeshComp->GetFName());
		}
		return MeshNameArr;
	}
	
	UFUNCTION(CallInEditor)
	const TArray<FString> GetMemberList() const;

	template<typename E>
	void ExecuteEffectEvent(E EventEnum, FVector Offset = FVector()) const
	{
		FString EventString = EnumHelper::EnumToString<E>(EventEnum);
		ExecuteEffectEvent(EventString, Offset);
	}

	UFUNCTION(BlueprintCallable)
	void ExecuteEffectEvent(const FString& EventName, FVector Offset = FVector::ZeroVector) const;

	template<typename E>
	void ExecuteEntityAcyncEffectEvent(E EventEnum) const
	{
		FString EventString = EnumHelper::EnumToString<E>(EventEnum);
		ExecuteEntityAcyncEffectEvent(EventString);
	}
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "FXGroup")
	TMap<FName, FFXGroup> FXGroupMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialGroup")
	TMap<FName, FEffectMaterialGroup> MaterialGruopMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundGroup")
	TMap<FName, FEffectSoundGroup> SoundGroupMap;

private:
	class IEffectableActor* OwnerActor;
	TArray<class UMeshComponent*> Meshs;
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnEffectEvent"))
	FEffectEvent EffectEvent;
};
