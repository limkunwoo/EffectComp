// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.


#include "MVEffectGroup.h"
#include "MVEffectComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BlueprintHelper/MVEffectUtil.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Internationalization/Regex.h"
#include "Engine/Texture2D.h"

namespace mv::EffectGroupUtill
{
	bool ParseProperty(UObject* Owner, FString InPropertyPath, FProperty*& OutProperty, void*& OutContainerAddress, void*& OutValueAddress)
	{
		if (InPropertyPath.Contains(TEXT("::")))
		{
			InPropertyPath = InPropertyPath.Right(InPropertyPath.Find(TEXT("::")));
		}
		UClass* OwnerClass = Owner->GetClass();

		OutContainerAddress = nullptr;
		OutValueAddress = nullptr;
		OutProperty = nullptr;

		TArray<FString> Parsed;
		InPropertyPath.ParseIntoArray(Parsed, TEXT("."), true);

		if (Parsed.Num() == 0 || InPropertyPath.IsEmpty() || !OwnerClass->FindPropertyByName(FName(Parsed[0])))
		{
			ensure(false);
			
			return false;
		}

		OutProperty = OwnerClass->FindPropertyByName(FName(Parsed[0]));
		OutContainerAddress = Owner;
		OutValueAddress = nullptr;

		OutValueAddress = OutProperty->ContainerPtrToValuePtr<void>(OutContainerAddress);

		if (Parsed.Num() == 2)
		{
			OutContainerAddress = OutValueAddress;
			OutProperty = CastField<FStructProperty>(OutProperty)->Struct->FindPropertyByName(FName(Parsed[1]));
			if (!ensure(OutProperty))
			{
				return false;
			}
			OutValueAddress = OutProperty->ContainerPtrToValuePtr<void>(OutContainerAddress);
		}
		return true;
	}

	static FString DefaultOption_BeginPlay(TEXT("Default::BeginPlay"));
}
using namespace mv::EffectGroupUtill;

struct Activate_Param
{
	FEffectGroup& ThisGroup;
};

void UEffectGroupProxy::Init(FEffectGroup* Client)
{
	ClientGroup = Client;
}

void UEffectGroupProxy::OnEffectEvent(FString Condiotion, FVector Offset)
{
	if (ClientGroup->bUseRawStringCondition)
	{
		if (ClientGroup->RawStringCondition == Condiotion)
		{
			ClientGroup->SetActive(true);
		}
	}
	else if (ClientGroup->ActiveCondition.Contains(TEXT("Event::")))
	{
		if (ClientGroup->ActiveCondition == TEXT("Event::") + Condiotion)
		{
			ClientGroup->SetActive(true);
		}
	}
}

FEffectGroup& UEffectGroupProxy::GetEffectGroup()
{
	return *ClientGroup;
}

void UEffectGroupProxy::SetParam(const FParamData& Param)
{
	ClientGroup->SetParam(Param);
}

void UEffectGroupProxy::SetActive(bool bNewActive)
{
	ClientGroup->SetActive(bNewActive);
}

void FFXGroup::Init(class UMVEffectComponent* InComponent, FName InGroupName)
{
	FEffectGroup::Init(InComponent, InGroupName);

	if (!EffectComponent)
	{
		return;
	}

	if (Sockets.IsEmpty())
	{
		//ensureMsgf(false, TEXT("Sockets empty. Effect will be Attach to Root"));
		Sockets.Add(FName(TEXT("Root")));
	}
	for (const FName& SocketName : Sockets)
	{
		if (bAttach)
		{
			if (UNiagaraComponent* CreatedNiagaraComponent = MVEffectUtil::SpawnSystemAttached(Niagara, FindMeshBySocketname(SocketName), SocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, false, ENCPoolMethod::None, false))
			{
				NiagaraComponents.Add(CreatedNiagaraComponent);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed Create Niagara"));
			}
		}
	}

	for (const FParamData& Param : Params)
	{
		if (Param.ValueType == EValueType::Static)
		{
			SetParam(Param);
		}
	}

	if (ActiveCondition == DefaultOption_BeginPlay)
	{
		SetActive(true);
	}
}

void FFXGroup::SetActive(bool bNewActive)
{
	if (bAttach)
	{
		for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
		{
			NiagaraComponent->SetRelativeLocation(Offset);
			NiagaraComponent->SetActive(bNewActive);
			Offset = FVector::ZeroVector;
		}
	}
	else
	{
		for (const FName& SocketName : Sockets)
		{
			if (!bNewActive) return;

			UNiagaraComponent* NiagaraComponent = MVEffectUtil::SpawnSystemAtLocation(EffectComponent, Niagara, FindMeshBySocketname(SocketName)->GetSocketLocation(SocketName) + Offset, FindMeshBySocketname(SocketName)->GetSocketRotation(SocketName));
			if (!NiagaraComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed Create Niagara"));
			}
		}
	}
}

void FFXGroup::UpdateGroup()
{
	FEffectGroup::UpdateGroup();
	if (bUseParameterSend)
	{
		for (FParamData& Param : Params)
		{
			if (Param.ValueType == EValueType::Bind)
			{
				Param.SetProperty(EffectComponent->GetOwner());
				SetParam(Param);
			}
		}
	}
}

void FFXGroup::SetParam(const FParamData& ParamData)
{
	for (UNiagaraComponent* Component : NiagaraComponents)
	{
		switch (ParamData.CppType)
		{
		case EStaticValueType::None:
			break;
		case EStaticValueType::Vector:
			Component->SetVectorParameter(ParamData.ParamName, ParamData.VectorValue);
			break;
		case EStaticValueType::Color:
			Component->SetColorParameter(ParamData.ParamName, ParamData.Color);
			break;
		case EStaticValueType::Bool:
			Component->SetBoolParameter(ParamData.ParamName, ParamData.BoolValue);
			break;
		case EStaticValueType::Float:
			Component->SetFloatParameter(ParamData.ParamName, ParamData.FloatValue);
			break;
		case EStaticValueType::Texture:
			Component->SetVariableTexture(ParamData.ParamName, ParamData.TextureValue);
			break;
		default:
			break;
		}
	}
}

void FEffectMaterialGroup::Init(UMVEffectComponent* InComponent, FName InGroupName)
{
	FEffectGroup::Init(InComponent, InGroupName);
	if (EffectedMeshNames.IsEmpty()) return;

	EffectedMeshs.Empty();

	TArray<UMeshComponent*> AllMeshs;
	EffectComponent->GetOwner()->GetComponents<UMeshComponent*>(AllMeshs);
	
	for (const FName& MeshName : EffectedMeshNames)
	{
		if (MeshName.IsNone())
		{
			ensureAlways(false);
			continue;
		}
		UMeshComponent* Mesh = Cast<UMeshComponent>(EffectComponent->GetOwner()->GetDefaultSubobjectByName(MeshName));
		if (Mesh)
		{
			EffectedMeshs.Emplace(Mesh);
		}
	}
	if (EffectedMeshs.IsEmpty() && AllMeshs.IsValidIndex(0))
	{
		EffectedMeshs.Emplace(AllMeshs[0]);
	}
	ensureAlways(!EffectedMeshs.IsEmpty());
}

void FEffectMaterialGroup::UpdateGroup()
{
	FEffectGroup::UpdateGroup();
	if (!bActivated) return;

	for (FParamData& Param : ActiveParams)
	{
		if (Param.ValueType == EValueType::Bind)
		{
			Param.SetProperty(EffectComponent->GetOwner());
			SetParam(Param);
		}
	}
}

void FEffectMaterialGroup::SetParam(const FParamData& Param)
{
	AActor* Owner = EffectComponent->GetOwner();
	for (TPair<UMeshComponent*, FMaterialSwapData>& Elem : SwapData)
	{
		for (UMaterialInstanceDynamic* MInstanceDynamic : Elem.Value.CreatedMIDArr)
		{
			switch (Param.CppType)
			{
			case EStaticValueType::Float:
				MInstanceDynamic->SetScalarParameterValue(Param.ParamName, Param.FloatValue);
				break;
			case EStaticValueType::Bool:
				MInstanceDynamic->SetScalarParameterValue(Param.ParamName, Param.BoolValue);
				break;
			case EStaticValueType::Vector:
				MInstanceDynamic->SetVectorParameterValue(Param.ParamName, Param.VectorValue);
				break;
			case EStaticValueType::Texture:
				MInstanceDynamic->SetTextureParameterValue(Param.ParamName, Param.TextureValue);
				break;
			case EStaticValueType::Color:
				MInstanceDynamic->SetVectorParameterValue(Param.ParamName, Param.Color);
				break;
			}
		}
	}
}
void FEffectMaterialGroup::SetActive(bool bNewActive)
{
	if (bActivated == bNewActive) return;
	bActivated = bNewActive;
	if (bNewActive)
	{
		if (EffectedMeshs.IsEmpty()) return;

		

		//SwapMaterial
		for (UMeshComponent* Elem : EffectedMeshs)
		{
			FMaterialSwapData Data;
			Data.Clear();
			Data.Init(Elem, bAllSlotEffect ? Elem->GetMaterialSlotNames() : SlotNames, MaintainParam, bUseTempMaterial ? TempMaterial : nullptr);
			Data.Swap();
			SwapData.Emplace(Elem, MoveTemp(Data));
		}

		for (const FParamData& Param : ActiveParams)
		{
			if (Param.ValueType == EValueType::Static)
			{
				SetParam(Param);
			}
		}
	}
	else
	{
		for (const FParamData& Param : DeActiveParams)
		{
			if (Param.ValueType == EValueType::Static && !bUseTempMaterial)
			{
				SetParam(Param);
			}
		}
		for (TPair<UMeshComponent*, FMaterialSwapData>& Elem : SwapData)
		{
			////Revert
			Elem.Value.Revert();
		}
		
	}

}

void FEffectSoundGroup::Init(UMVEffectComponent* InComponent, FName InGroupName)
{
	FEffectGroup::Init(InComponent, InGroupName);

	if (!EffectComponent)
	{
		return;
	}

	if (Sockets.IsEmpty())
	{
		Sockets.Add(FName(TEXT("Root")));
	}
	
	if (bAttach)
	{
		for (const FName& SocketName : Sockets)
		{
			UMeshComponent* AttachMesh = FindMeshBySocketname(SocketName);
			if (UAudioComponent* CreatedSoundComponent = UGameplayStatics::SpawnSoundAttached(SoundAsset, FindMeshBySocketname(SocketName), SocketName, FVector::ZeroVector, EAttachLocation::SnapToTarget, true, 1.0, 1.0f, 0.f, nullptr, nullptr, false))
			{
				CreatedSoundComponent->Stop();
				SoundComponents.Add(CreatedSoundComponent);
			}
		}
	}

	for (const FParamData& Param : Params)
	{
		if (Param.ValueType == EValueType::Static)
		{
			SetParam(Param);
		}
	}

	if (ActiveCondition == DefaultOption_BeginPlay)
	{
		SetActive(true);
	}
}

void FEffectSoundGroup::SetActive(bool bNewActive)
{
	if (!bAttach && bNewActive)
	{ 
		for (const FName& SocketName : Sockets)
		{
			FVector Location = FindMeshBySocketname(SocketName)->GetSocketLocation(SocketName);
			UGameplayStatics::PlaySoundAtLocation(EffectComponent, SoundAsset, Location);
		}
	}
	if (bAttach)
	{
		for (UAudioComponent* AudioComponent : SoundComponents)
		{
			if (bNewActive)
			{
				if (AudioComponent->IsPlaying() && !bResetWhenActivated)
				{
					return;
				}
				AudioComponent->Play(0.f);
			}
			else
			{
				AudioComponent->Stop();
			}
		}
	}
}

void FEffectSoundGroup::UpdateGroup()
{
	FEffectGroup::UpdateGroup();
	if (bUseParameterSend)
	{
		for (FParamData& Param : Params)
		{
			if (Param.ValueType == EValueType::Bind)
			{
				Param.SetProperty(EffectComponent->GetOwner());
				SetParam(Param);
			}
		}
	}
}

void FEffectSoundGroup::SetParam(const FParamData& Param)
{
	for (UAudioComponent* Component : SoundComponents)
	{
		switch (Param.CppType)
		{
		case EStaticValueType::Bool:
			Component->SetBoolParameter(Param.ParamName, Param.BoolValue);
			break;
		case EStaticValueType::Float:
			Component->SetFloatParameter(Param.ParamName, Param.FloatValue);
			break;
		case EStaticValueType::Int:
			Component->SetIntParameter(Param.ParamName, Param.FloatValue);
			break;
		case EStaticValueType::Trigger:
			if (!Param.Trigger.IsNone())
			{
				Component->SetTriggerParameter(Param.Trigger);
				const_cast<FParamData&>(Param).Trigger = NAME_None;
			}
		default:
			break;
		}
	}
	
}

void FEffectGroup::Init(UMVEffectComponent* InComponent, FName InGroupName)
{
	GroupName = InGroupName;
	EffectComponent = InComponent;

	HandlerProxy = NewObject<UEffectGroupProxy>(EffectComponent);
	HandlerProxy->Init(this);
	EffectComponent->EffectEvent.AddDynamic(HandlerProxy, &UEffectGroupProxy::OnEffectEvent); 
}

void FEffectGroup::UpdateGroup()
{
	FProperty* Founded = nullptr;
	void* ContainerAddress = nullptr;
	void* ValueAddress = nullptr;

	FRegexPattern PropertyRegEx(TEXT("^Property(\\([\\w]+\\))*::"));
	FRegexPattern FunctionRegEx(TEXT("^Function::"));
	FRegexPattern EventRegEx(TEXT("^Event::"));

	FRegexMatcher Matcher(PropertyRegEx, ActiveCondition);
	if (Matcher.FindNext())
	{
		TArray<FString> Parsed;
		ActiveCondition.ParseIntoArray(Parsed, TEXT("::"));

		ParseProperty(EffectComponent->GetOwner(), Parsed[1], Founded, ContainerAddress, ValueAddress);
		bool Value = false;
		if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Founded))
		{
			Value = *(bool*)ValueAddress;
			
		}
		else if (FNumericProperty* Numeric = CastField<FNumericProperty>(Founded))
		{
			FString Number = Numeric->GetNumericPropertyValueToString(ValueAddress);
			Value = (bool)FCString::Atof(*Number);
		}
		if (bConditionAsReverse)
		{
			Value = !Value;
		}
		if (bCachedPrevValue != Value)
		{
			SetActive(Value);
		}
		bCachedPrevValue = Value;
	}
	else if (ActiveCondition.Contains(TEXT("Function::")))
	{
		TArray<FString> Parsed;
		ActiveCondition.ParseIntoArray(Parsed, TEXT("::"));

		FName FunctionName = FName(Parsed[1]);

		UObject* Owner = EffectComponent->GetOwner();
		UFunction* Func = Owner->FindFunction(FunctionName);
		if (ensure(Func))
		{
			struct Params { bool Value; };

			Params Ret;
			Owner->ProcessEvent(Func, &Ret);
			if (bCachedPrevValue != Ret.Value)
			{
				SetActive(Ret.Value);
			}
			bCachedPrevValue = Ret.Value;
		}
	}
}


UMeshComponent* FEffectGroup::FindMeshBySocketname(FName SocketName)
{
	/*if (UMeshComponent* EffectedMesh = Cast<UMeshComponent>(EffectComponent->GetOwner()->GetDefaultSubobjectByName(EffectedMeshName)))
	{
		if (EffectedMesh->DoesSocketExist(SocketName))
		{
			return EffectedMesh;
		}
	}*/

	for (UMeshComponent* Mesh : EffectComponent->Meshs)
	{
		if (Mesh->DoesSocketExist(SocketName))
		{
			return Mesh;
		}
	}
	ensureAlways(EffectComponent->Meshs.Num() > 0);
	return EffectComponent->Meshs.Num() > 0 ? EffectComponent->Meshs[0] : nullptr;
}

void FMaterialSwapData::Swap()
{
	for (int32 i = 0; i < OriginArr.Num(); i++)
	{
		Mesh->SetMaterial(SlotIndices[i], CreatedMIDArr[i]);
	}
}

void FMaterialSwapData::Revert()
{
	for (int32 i = 0; i < CreatedMIDArr.Num(); i++)
	{
		Mesh->SetMaterial(SlotIndices[i], OriginArr[i]);
	}
}

void FMaterialSwapData::Init(class UMeshComponent* InEffectedMesh, TArray<FName> InEffectedSlots, const TArray<FMaterialMaintainParam>& InMoveParams, UMaterialInterface* TempMI)
{
	Mesh = InEffectedMesh;

	for (const FName& SlotName : InEffectedSlots)
	{
		int32 SlotIndex = Mesh->GetMaterialIndex(SlotName);
		if (SlotIndex == INDEX_NONE)
		{
 			//ensureAlways(false);
			continue;
		}
		UMaterialInterface* OriginMI = InEffectedMesh->GetMaterial(SlotIndex);
		if (!OriginMI)
		{
			//ensureAlways(OriginMI);
			continue;
		}

		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(OriginMI);
		if (!MID)
		{
			MID = Mesh->CreateAndSetMaterialInstanceDynamic(SlotIndex);
		}
		OriginArr.Add(MID);

		if (TempMI)
		{
			MID = UMaterialInstanceDynamic::Create(TempMI, InEffectedMesh);
		}
		CreatedMIDArr.Add(MID);
		SlotIndices.Add(SlotIndex);
		if (TempMI)
		{
			for (const FMaterialMaintainParam& Param : InMoveParams)
			{
				switch (Param.CppType)
				{
				case EStaticValueType::Vector:
				case EStaticValueType::Color:
				{
					FLinearColor MaintainValue;
					OriginMI->GetVectorParameterValue(FHashedMaterialParameterInfo(Param.SouceParamName), MaintainValue);
					MID->SetVectorParameterValue(Param.DestParamName, MaintainValue);
				}
				break;
				case EStaticValueType::Bool:
				case EStaticValueType::Float:
				{
					float MaintainValue;
					OriginMI->GetScalarParameterValue(FHashedMaterialParameterInfo(Param.SouceParamName), MaintainValue);
					MID->SetScalarParameterValue(Param.DestParamName, MaintainValue);
				}
				break;
				case EStaticValueType::Texture:
				{
					UTexture* MaintainValue;
					OriginMI->GetTextureParameterValue(FHashedMaterialParameterInfo(Param.SouceParamName), MaintainValue);
					MID->SetTextureParameterValue(Param.DestParamName, MaintainValue);
				}
				break;
				default:
					break;
				}
			}
		}
	}
}


void FParamData::SetProperty(UObject* Owner)
{
	void* ContainerAddress = nullptr;
	void* ValueAddress = nullptr;
	FProperty* Founded = nullptr;
	if (ParseProperty(Owner, MemberName, Founded, ContainerAddress, ValueAddress))
	{
		if (FNameProperty* TriggerProperty = CastField<FNameProperty>(Founded))
		{
			SetTriggerValue(*(FName*)ValueAddress);
		}
		else if (Founded->GetCPPType() == TEXT("FVector"))
		{
			SetVectorValue(*(FVector*)ValueAddress);
		}
		else if (Founded->GetCPPType() == TEXT("FLinearColor"))
		{
			SetColorValue(*(FLinearColor*)ValueAddress);
		}
		else if (FBoolProperty* Bool = CastField<FBoolProperty>(Founded))
		{
			SetBoolValue(*(bool*)ValueAddress);
		}
		else if (FNumericProperty* Numeric = CastField<FNumericProperty>(Founded))
		{
			if (Numeric->IsFloatingPoint())
			{
				SetFloatValue(Numeric->GetFloatingPointPropertyValue(ValueAddress));
			}
			else if (Numeric->IsInteger())
			{
				SetIntValue(Numeric->GetSignedIntPropertyValue(ValueAddress));
			}
		}
		else if (FObjectProperty* Object = CastField<FObjectProperty>(Founded))
		{
			if (Object->PropertyClass->IsChildOf(UTexture::StaticClass()))
			{
				SetTextureValue(Cast<UTexture>(Object->GetObjectPropertyValue(ValueAddress)));
			}
		}
	}
}
