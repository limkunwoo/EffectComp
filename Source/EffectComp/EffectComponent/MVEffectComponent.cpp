// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.


#include "MVEffectComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "EffectableActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture.h"

// Sets default values for this component's properties
UMVEffectComponent::UMVEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMVEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	GetOwner()->GetComponents<UMeshComponent>(Meshs);
	OwnerActor = Cast<IEffectableActor>(GetOuter());
	for (TPair<FName, FFXGroup>& Pair : FXGroupMap)
	{
		Pair.Value.Init(this, Pair.Key);
	}

	for (TPair<FName, FEffectMaterialGroup>& Pair : MaterialGruopMap)
	{
		Pair.Value.Init(this, Pair.Key);
	}
	for (TPair<FName, FEffectSoundGroup>& Pair : SoundGroupMap)
	{
		Pair.Value.Init(this, Pair.Key);
	}
	AddTickPrerequisiteActor(GetWorld()->GetFirstPlayerController());
	GetOwner()->AddTickPrerequisiteComponent(this);
}

const TArray<FString> UMVEffectComponent::GetMemberList() const
{
	static FString VectorCppName = FString("FVector");
	static FString ColorCppName = FString("FLinearColor");
	static FString UberGraph = FString(TEXT("FPointerToUberGraphFrame"));
	auto IsSupportedType = [](FProperty* Prop)->bool 
		{
			bool bIsSupport = Prop->GetClass()->IsChildOf(FBoolProperty::StaticClass()) || Prop->GetClass()->IsChildOf(FNumericProperty::StaticClass()) || Prop->GetCPPType().Equals(VectorCppName) || Prop->GetCPPType().Equals(ColorCppName) || Prop->GetClass()->IsChildOf(FNameProperty::StaticClass());
			if (FObjectProperty* Object = CastField<FObjectProperty>(Prop))
			{
				if (Object->PropertyClass->IsChildOf(UTexture::StaticClass()))
				{
					bIsSupport = true;
				}
			}
			return bIsSupport;
		};
	auto SearchProperty = [IsSupportedType](UClass* OwnerClass, TArray<FString>& OutString)
		{
			for (TFieldIterator<FProperty> It(OwnerClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				if (IsSupportedType(*It))
				{
					OutString.Emplace(It->GetNameCPP());
				}
				else if (It->IsA(FStructProperty::StaticClass()) && !It->GetCPPType().Equals(UberGraph))
				{
					FStructProperty* Struct = CastField<FStructProperty>(*It);
					for (TFieldIterator<FProperty> InnerIt(Struct->Struct, EFieldIteratorFlags::ExcludeSuper); InnerIt; ++InnerIt)
					{
						if (IsSupportedType(*InnerIt))
						{
							OutString.Emplace(Struct->GetNameCPP() + TEXT(".") + InnerIt->GetNameCPP());
						}
					}
				}
			}
		};
	TArray<FString> OutString;
	UClass* Class = GetOwner()->GetClass();
	while (Class ->IsChildOf(AActor::StaticClass())  && Class != AActor::StaticClass() && Class != ACharacter::StaticClass() && Class != APawn::StaticClass())
	{
		SearchProperty(Class, OutString);
		Class = Class->GetSuperClass();
	}
	
	return OutString;
}

void UMVEffectComponent::ExecuteEffectEvent(const FString& EventName, FVector Offset) const
{
	EffectEvent.Broadcast(EventName, Offset);
}

#if WITH_EDITOR
void UMVEffectComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UMVEffectComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static const FName NAME_MemberValue(TEXT("MemberName"));
	static const FString NAME_FXParamArrName(TEXT("Params"));
	static const FString NAME_FXGroupArrName(TEXT("FXGroupMap"));
	static const FString NAME_MaterialGroupArrName(TEXT("MaterialGruopMap"));
	static const FString NAME_SoundGroupArrName(TEXT("SoundGroupMap"));
	static const FName NAME_SocketArr(TEXT("Sockets"));
	static const FName NAME_FXParamName(TEXT("ParamName"));

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	FProperty* Property = PropertyChangedEvent.PropertyChain.GetActiveNode()->GetValue();
	FProperty* MemberProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue();

	if (!Property || !MemberProperty) return;
	if(GetOwner()->bIsEditorPreviewActor) return;
	const FName PropertyName = Property->GetFName();
	if (PropertyName == NAME_MemberValue)
	{
		int32 GroupIdx;
		int32 ParamIdx;
		FParamData* Param = nullptr;

		TArray<FName> Arr;
		if (MemberProperty->GetName() == NAME_FXGroupArrName)
		{
			FXGroupMap.GenerateKeyArray(Arr);
			GroupIdx = PropertyChangedEvent.GetArrayIndex(NAME_FXGroupArrName);
			ParamIdx = PropertyChangedEvent.GetArrayIndex(NAME_FXParamArrName);
			if (!Arr.IsValidIndex(GroupIdx)) return;
			FName GroupKey = Arr[GroupIdx];
			if (!FXGroupMap.Contains(GroupKey))
			{
				return;
			}
			Param = &(FXGroupMap[GroupKey].Params[ParamIdx]);
		}
		else if (MemberProperty->GetName() == NAME_MaterialGroupArrName)
		{
			MaterialGruopMap.GenerateKeyArray(Arr);
			GroupIdx = PropertyChangedEvent.GetArrayIndex(NAME_MaterialGroupArrName);
			
			if (!Arr.IsValidIndex(GroupIdx)) return;
			FName GroupKey = Arr[GroupIdx];
			FProperty* PrevValue = PropertyChangedEvent.PropertyChain.GetActiveNode()->GetPrevNode()->GetValue();

			if (!MaterialGruopMap.Contains(GroupKey))
			{
				return;
			}
			FEffectMaterialGroup& MetrialGroup = MaterialGruopMap[GroupKey];
			if (PrevValue->GetName() == TEXT("ActiveParams"))
			{
				ParamIdx = PropertyChangedEvent.GetArrayIndex(FString(TEXT("ActiveParams")));
				
				if (!MetrialGroup.ActiveParams.IsValidIndex(ParamIdx))
				{
					return;
				}
				
				Param = &MetrialGroup.ActiveParams[ParamIdx];
			}
			else if(PrevValue->GetName() == TEXT("DeActiveParams"))
			{
				ParamIdx = PropertyChangedEvent.GetArrayIndex(FString(TEXT("DeActiveParams")));
				if (!MetrialGroup.DeActiveParams.IsValidIndex(ParamIdx))
				{
					return;
				}
				Param = &MetrialGroup.DeActiveParams[ParamIdx];
			}
		}
		else if (MemberProperty->GetName() == NAME_SoundGroupArrName)
		{
			SoundGroupMap.GenerateKeyArray(Arr);
			GroupIdx = PropertyChangedEvent.GetArrayIndex(NAME_SoundGroupArrName);
			ParamIdx = PropertyChangedEvent.GetArrayIndex(NAME_FXParamArrName);
			if (!Arr.IsValidIndex(GroupIdx)) return;
			FName GroupKey = Arr[GroupIdx];
			if (!SoundGroupMap.Contains(GroupKey) || !SoundGroupMap[GroupKey].Params.IsValidIndex(ParamIdx))
			{
				return;
			}
			Param = &(SoundGroupMap[GroupKey].Params[ParamIdx]);
		}
		AActor* Owner = GetOwner();
		if (Owner)
		{
			Param->SetProperty(Owner);
		}
	}

	if (PropertyName == NAME_SocketArr)
	{
		int32 GroupIdx;
		int32 SocketIdx;
		FParamData* Param = nullptr;

		TArray<FName> Arr;
		if (MemberProperty->GetName() == NAME_FXGroupArrName)
		{
			FXGroupMap.GenerateKeyArray(Arr);
			GroupIdx = PropertyChangedEvent.GetArrayIndex(NAME_FXGroupArrName);
			SocketIdx = PropertyChangedEvent.GetArrayIndex(NAME_SocketArr.ToString());
			if (GroupIdx < 0 || SocketIdx < 0 || Arr.IsValidIndex(GroupIdx)) return;
			FName GroupKey = Arr[GroupIdx];
			
			if (FXGroupMap.Contains(GroupKey))
			{
				FFXGroup& Group = FXGroupMap[GroupKey];
				if (Group.Sockets.IsValidIndex(SocketIdx))
				{
					FName InputSocketName = Group.Sockets[SocketIdx];
					bool IsSocketExist = false;

					TArray<UMeshComponent*> MeshArr;
					GetOwner()->GetComponents<UMeshComponent>(MeshArr);
					for (UMeshComponent* Mesh : MeshArr)
					{
						if (!Mesh->DoesSocketExist(InputSocketName))
						{
							IsSocketExist = false;
							continue;
						}
						else
						{
							IsSocketExist = true;
							break;
						}
					}
					if (!IsSocketExist)
					{
						FXGroupMap[GroupKey].Sockets[SocketIdx] = FName(TEXT("Invalid SocketName retry "));
					}
				}
			}
			
			
		}
	}
}
#endif

// Called every frame
void UMVEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (TPair<FName, FFXGroup>& Pair : FXGroupMap)
	{
		Pair.Value.UpdateGroup();
	}
	for (TPair<FName, FEffectMaterialGroup>& Pair : MaterialGruopMap)
	{
		Pair.Value.UpdateGroup();
	}
	for (TPair<FName, FEffectSoundGroup>& Pair : SoundGroupMap)
	{
		Pair.Value.UpdateGroup();
	}
	// ...
}

void UMVEffectComponent::ActivateEffectGroup(EEffectType EffectType, FName GroupName, bool bNewActive)
{
	switch (EffectType)
	{
	case EEffectType::Niagara:
		if (ensure(FXGroupMap.Contains(GroupName)))
		{
			FXGroupMap[GroupName].SetActive(bNewActive);
		}
		break;
	case EEffectType::Material:
		if (ensure(MaterialGruopMap.Contains(GroupName)))
		{
			MaterialGruopMap[GroupName].SetActive(bNewActive);
		}
		break;
	case EEffectType::Sound:
		if (ensure(SoundGroupMap.Contains(GroupName)))
		{
			SoundGroupMap[GroupName].SetActive(bNewActive);
		}
		break;
	case EEffectType::Light:
		break;
	default:
		break;
	}
}

void UMVEffectComponent::SetParameter(EEffectType EffectType, FName GroupName, FParamData Param)
{
	switch (EffectType)
	{
	case EEffectType::Niagara:
		if (ensure(FXGroupMap.Contains(GroupName)))
		{
			FXGroupMap[GroupName].SetParam(Param);
		}
		break;
	case EEffectType::Material:
		if (ensure(MaterialGruopMap.Contains(GroupName)))
		{
			MaterialGruopMap[GroupName].SetParam(Param);
		}
		break;
	case EEffectType::Sound:
		if (ensure(SoundGroupMap.Contains(GroupName)))
		{
			SoundGroupMap[GroupName].SetParam(Param);
		}
		break;
	case EEffectType::Light:
		break;
	default:
		break;
	}
}

const TArray<FName> UMVEffectComponent::GetActiveOptions() const
{
	AActor* Owner = GetOwner();

	static FString UberGraph = FString(TEXT("FPointerToUberGraphFrame"));
	auto IsSupportedType = [](FProperty* Prop)->bool
		{
			bool bIsSupport = Prop->GetClass()->IsChildOf(FBoolProperty::StaticClass()) || Prop->GetClass()->IsChildOf(FNumericProperty::StaticClass());
			return bIsSupport;
		};
	auto SearchProperty = [IsSupportedType](UClass* OwnerClass, TArray<FName>& OutString)
		{
			FString PrefixFormat = TEXT("Property({0})::");
			
			for (TFieldIterator<FProperty> It(OwnerClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				if (IsSupportedType(*It))
				{
					FStringFormatOrderedArguments Args;
					Args.Add(It->GetCPPType());
					OutString.Emplace(FName(FString::Format(*PrefixFormat, Args) + It->GetNameCPP()));
				}
				else if (It->IsA(FStructProperty::StaticClass()) && !It->GetCPPType().Equals(UberGraph))
				{
					FStructProperty* Struct = CastField<FStructProperty>(*It);
					for (TFieldIterator<FProperty> InnerIt(Struct->Struct, EFieldIteratorFlags::ExcludeSuper); InnerIt; ++InnerIt)
					{
						if (IsSupportedType(*InnerIt))
						{
							FStringFormatOrderedArguments Args;
							Args.Add(InnerIt->GetCPPType());
							OutString.Emplace(FName(FString::Format(*PrefixFormat, Args) + Struct->GetNameCPP() + TEXT(".") + InnerIt->GetNameCPP()));
						}
					}
				}
			}
		};
	auto SearchFunction = [](UClass* OwnerClass, TArray<FName>& OutString)
		{
			for (TFieldIterator<UFunction> It(OwnerClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				UFunction* Func = *It;
				if (It->NumParms == 1)
				{
					if (Func->PropertyLink->PropertyFlags & (CPF_ReturnParm | CPF_OutParm))
					{
						if (FBoolProperty* RetPropAsBool = CastField<FBoolProperty>(Func->PropertyLink))
						{
							OutString.Emplace(FName(TEXT("Function::") + It->GetName()));
						}
					}

				}
			}
		};
	TArray<FName> OutString;
	IEffectableActor* Outer = Cast<IEffectableActor>(Owner);
	if (Outer)
	{
		TArray<FName> Events = Outer->GetEffectEventOptions();
		for (FName& Elem : Events)
		{
			OutString.Emplace(FName(TEXT("Event::") + Elem.ToString()));
		}
	}

	UClass* Class = GetOwner()->GetClass();
	while (Class->IsChildOf(AActor::StaticClass()) && Class != AActor::StaticClass() && Class != ACharacter::StaticClass() && Class != APawn::StaticClass())
	{
		SearchProperty(Class, OutString);
		SearchFunction(Class, OutString);
		Class = Class->GetSuperClass();
	}
	OutString.Sort([](const FName& Left, const FName& Right) 
		{
			return Left.ToString() < Right.ToString();
		}
	);

	OutString.Insert(FName(TEXT("Default::BeginPlay")), 0);
	return OutString;
}

