// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "MVEffectGroup.generated.h"

class UMaterialInstanceDynamic;

/**
 *
 */

UENUM()
enum class EEffectType : uint8
{
	Niagara,
	Material,
	Sound,
	Light
};
UENUM()
enum class EValueType : uint8
{
	Static,
	Bind,
};
UENUM()
enum class EStaticValueType : uint8
{
	None,
	Vector,
	Color,
	Bool,
	Float,
	Int,
	Texture,
	Trigger,
};


USTRUCT(Blueprintable)
struct FParamData
{
	friend struct FFXGroup;
	friend class UMVEffectComponent;
	friend struct FEffectMaterialGroup;
	friend struct FEffectSoundGroup;
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Float", EditConditionHides, DisplayAfter = "CppType"))
	float FloatValue = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Int", EditConditionHides, DisplayAfter = "CppType"))
	int32 IntValue = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Vector", EditConditionHides, DisplayAfter = "CppType"))
	FVector VectorValue = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Bool", EditConditionHides, DisplayAfter = "CppType"))
	bool BoolValue = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Texture", EditConditionHides, DisplayAfter = "CppType"))
	UTexture* TextureValue = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Color", EditConditionHides, DisplayAfter = "CppType"))
	FLinearColor Color = FLinearColor::Black;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static && CppType == EStaticValueType::Trigger", EditConditionHides, DisplayAfter = "CppType"))
	FName Trigger = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ValueType == EValueType::Static", DisplayAfter = "ParamName"))
	EStaticValueType CppType = EStaticValueType::None;

	bool bIsSet = false;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EValueType ValueType = EValueType::Static;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "EValueType::Bind == ValueType", EditConditionHides, GetOptions = "GetMemberList", DisplayAfter = "ValueType", DisplayPriority = "1"))
	FString MemberName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayAfter = "ValueType"))
	FName ParamName;

	void SetVectorValue(FVector InValue) { VectorValue = InValue; CppType = EStaticValueType::Vector; bIsSet = true; }
	void SetFloatValue(float InValue) { FloatValue = InValue; CppType = EStaticValueType::Float; bIsSet = true; }
	void SetIntValue(int32 InValue) { IntValue = InValue; CppType = EStaticValueType::Int; bIsSet = true; }
	void SetBoolValue(bool InBool) { BoolValue = InBool; CppType = EStaticValueType::Bool; bIsSet = true; }
	void SetColorValue(FLinearColor InColor) { Color = InColor; CppType = EStaticValueType::Color; bIsSet = true; }
	void SetTextureValue(UTexture* InTexture) { TextureValue = InTexture; CppType = EStaticValueType::Texture; bIsSet = true; }
	void SetTriggerValue(FName InTrigger) { Trigger = InTrigger; CppType = EStaticValueType::Trigger; bIsSet = true; }

	void SetProperty(UObject* Owner);
	bool IsSet() { return bIsSet; }
};

USTRUCT(BlueprintType)
struct FEffectGroup
{
	GENERATED_BODY()

	virtual ~FEffectGroup()
	{
	}
	/// <summary>
	///ActiveCondition이 Property 또는 Function 일때 조건을 not boolean으로 평가함.
	/// </summary>
	UPROPERTY(EditDefaultsOnly)
	bool bConditionAsReverse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (InlineEditConditionToggle, DisplayAfter = "bConditionAsReverse"))
	bool bUseRawStringCondition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bUseRawStringCondition", EditConditionHides, DisplayAfter = "bConditionAsReverse"))
	FString RawStringCondition;
	/// <summary>
	/// Event::EventName				단발성 이펙트에 적합. one-off, 이벤트가 발생했을때 트리거되고 신경쓰지 않음. ex) 
	/// Property(Type)::PropertyName	매 틱 PropertyName 변수를 평가한다. PropertyName이 true 인 동안 이펙트가 실행되고 false인 동안 꺼져 있는다.
	/// Function::FunctionName			입력이 없고, bool Type 하나만을 반환하는 함수들을 노출, FunctionName 함수의 반환값을 평가한다. 함수의 반환값이 true 인 동안 이펙트가 실행되고 false인 동안 꺼진다.
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (GetOptions = "GetActiveOptions", EditCondition = "!bUseRawStringCondition", EditConditionHides, DisplayAfter = "bConditionAsReverse"))
	FString ActiveCondition;

	UPROPERTY(BlueprintReadOnly)
	FName GroupName;
	UPROPERTY()
	class UEffectGroupProxy* HandlerProxy = nullptr;
	UPROPERTY()
	class UMVEffectComponent* EffectComponent = nullptr;

	FVector Offset;


	virtual void Init(class UMVEffectComponent* InComponent, FName InGroupName);
	virtual void SetActive(bool bNewActive) {}
	virtual void UpdateGroup();
	virtual void SetParam(const FParamData& Param) {}

protected:
	class UMeshComponent* FindMeshBySocketname(FName SocketName);
	bool bCachedPrevValue;
};
USTRUCT(BlueprintType)
struct FSocketAttachEffectGroup : public FEffectGroup
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayAfter = "Params"))
	TArray<FName> Sockets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayAfter = "Asset"))
	bool bAttach = true;
	/*UPROPERTY(BlueprintReadWrite, meta = (DisplayAfter = "bAttach"))
	bool bAutoDestroy;*/
};
UCLASS(BlueprintType, Blueprintable)
class UEffectGroupProxy : public UObject
{
	GENERATED_BODY()

public:
	void Init(struct FEffectGroup* Client);
	UFUNCTION()
	void OnEffectEvent(FString Condiotions, FVector Offset);

	UFUNCTION(BlueprintCallable, BlueprintGetter)
	FEffectGroup& GetEffectGroup();
	FEffectGroup* ClientGroup;

	UFUNCTION(BlueprintCallable)
	void SetParam(const FParamData& Param);
	UFUNCTION(BlueprintCallable)
	void SetActive(bool bNewActive);
};
USTRUCT(Blueprintable)
struct FFXGroup : public FSocketAttachEffectGroup
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayPriority = "0", DisplayName = "Asset"))
	class UNiagaraSystem* Niagara = nullptr;

	

	UPROPERTY()
	TArray<class UNiagaraComponent*> NiagaraComponents;

	UPROPERTY(EditDefaultsOnly, meta = (InlineEditConditionToggle))
	bool bUseParameterSend = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bUseParameterSend", DisplayAfter = "ActiveCondition"))
	TArray<FParamData> Params;
public:
	virtual void Init(class UMVEffectComponent* Component, FName InGroupName) override;
	virtual void SetActive(bool bNewActive) override;
	virtual void UpdateGroup() override;

	virtual void SetParam(const FParamData& Param) override;
};

USTRUCT(Blueprintable)
struct FMaterialMaintainParam
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EStaticValueType CppType = EStaticValueType::None;

	UPROPERTY(EditDefaultsOnly)
	FName SouceParamName;
	UPROPERTY(EditDefaultsOnly)
	FName DestParamName;
};

USTRUCT()
struct FMaterialSwapData
{
	friend struct FEffectMaterialGroup;
	GENERATED_BODY()
private:
	void Swap();
	void Revert();

	void Init(class UMeshComponent* InEffectedMesh, TArray<FName> InEffectedSlots, const TArray<FMaterialMaintainParam>& InMoveParams, UMaterialInterface* TempMI = nullptr);

	void Clear()
	{
		SlotIndices.Empty();
		CreatedMIDArr.Empty();
		OriginArr.Empty();
	}
	TArray<int32> SlotIndices;

	UPROPERTY()
	UMeshComponent* Mesh = nullptr;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> CreatedMIDArr;
	UPROPERTY()
	TArray<UMaterialInterface*> OriginArr;
	UPROPERTY()
	TArray<FParamData> OriginParams;
};



USTRUCT(Blueprintable)
struct FEffectMaterialGroup : public FEffectGroup
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (GetOptions = "GetAllMesh", DisplayPriority = "0"))
	TArray<FName> EffectedMeshNames;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayAfter = "EffectedMeshNames", DisplayPriority = "0"))
	bool bAllSlotEffect = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bAllSlotEffect", EditConditionHides, DisplayAfter = "EffectedMeshNames", DisplayPriority = "0"))
	TArray<FName> SlotNames;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayAfter = "SlotNames", InlineEditConditionToggle))
	bool bUseTempMaterial = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bUseTempMaterial", DisplayAfter = "SlotNames", DisplayPriority = "0"))
	class UMaterialInstance* TempMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayAfter = "ActiveCondition"))
	TArray<FParamData> ActiveParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayAfter = "ActiveCondition", EditCondition = "!bUseTempMaterial", EditConditionHides))
	TArray<FParamData> DeActiveParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bUseTempMaterial", DisplayAfter = "DeActiveParams", EditCondition = "bUseTempMaterial", EditConditionHides))
	TArray<FMaterialMaintainParam> MaintainParam;

	UPROPERTY()
	TArray<UMeshComponent*> EffectedMeshs;
	bool bActivated;

private:
	UPROPERTY()
	TMap<UMeshComponent*, FMaterialSwapData> SwapData;

public:
	virtual void Init(class UMVEffectComponent* Component, FName GroupName) override;
	virtual void SetActive(bool bNewActive) override;
	virtual void UpdateGroup() override;

	virtual void SetParam(const FParamData& Param) override;
};

USTRUCT(Blueprintable)
struct FEffectSoundGroup : public FSocketAttachEffectGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Asset"))
	class USoundBase* SoundAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bResetWhenActivated = true;

	

	UPROPERTY(EditDefaultsOnly, meta = (DisplayAfter = "ActiveCondition", InlineEditConditionToggle))
	bool bUseParameterSend = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bUseParameterSend", DisplayAfter = "ActiveCondition"))
	TArray<FParamData> Params;


	UPROPERTY()
	TArray<class UAudioComponent*> SoundComponents;
public:
	virtual void Init(class UMVEffectComponent* InComponent, FName InGroupName) override;
	virtual void SetActive(bool bNewActive) override;
	virtual void UpdateGroup() override;
	virtual void SetParam(const FParamData& Param) override;
};