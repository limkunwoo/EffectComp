// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Field.h"
#include "UObject/SparseDelegate.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakObjectPtr.h"

/**
 * 
 */
//template<typename CppType>
//struct TPropertyTypeTrait
//{
//	static constexpr bool IsEnum = TIsEnum<CppType>::Value;
//	static constexpr bool IsUObject = TIsDerivedFrom<CppType, UObject>::Value;
//	static constexpr bool IsStruct = !IsUObject && TIsClass<CppType>::Value;
//
//	template<typename T, bool IsEnum = TIsEnum<T>::Value, bool IsStruct = TIsClass<T>::Value && !TIsDerivedFrom<CppType, UObject>::Value, bool IsUObject = TIsDerivedFrom<CppType, UObject>::Value>
//	struct TypeResolve;
//
//	template<typename T>
//	struct TypeResolve<T, true, false, false>
//	{
//		using Type = FEnumProperty;
//
//		static CppType GetValue_InContainer(void* Container, Type* Property)
//		{
//			return static_cast<CppType>(Property->GetUnderlyingProperty()->GetSignedIntPropertyValue_InContainer(Container, Property));
//		}
//		static void SetValue_InContainer(void* Container, Type* Property, CppType Value)
//		{
//			Property->GetUnderlyingProperty()->SetIntPropertyValue(Container, (int64)Value);
//		}
//	};
//	template<typename T>
//	struct TypeResolve<T, false, true, false>
//	{
//		using Type = FStructProperty;
//
//		static CppType GetValue_InContainer(void* Container, Type* Property)
//		{
//			CppType DefaultStruct;
//			Property->GetValue_InContainer(Container, &DefaultStruct);
//			return DefaultStruct;
//		}
//
//		static void SetValue_InContainer(void* Container, Type* Property, CppType Value)
//		{
//			FProperty* BaseProperty = Property;
//			void* SubBuffer = Property->ContainerPtrToValuePtr<void>(Container);
//			Property->InitializeValue(SubBuffer);
//			Property->SetValue_InContainer(Container, &Value);
//		}
//	};
//	template<typename T>
//	struct TypeResolve<T, false, false, true>
//	{
//		using Type = FObjectProperty;
//		
//		static CppType GetValue_InContainer(void* Container, Type* Property)
//		{
//			return Cast<CppType>(Property->GetObjectPropertyValue_InContainer(Container));
//		}
//		static void SetValue_InContainer(void* Container, Type* Property, CppType* Value)
//		{
//			Property->SetObjectPropertyValue_InContainer(Container, Value);
//		}
//	};
//
//	using Type = typename TypeResolve<CppType>::Type;
//
//	static void SetValue_InContainer(void* Container, Type* Property, CppType Value)
//	{
//		TypeResolve<CppType>::SetValue_InContainer(Container, Property, Value);
//	}
//
//	static CppType GetValue_InContainer(void* Container, Type* Property)
//	{
//		return TypeResolve<CppType>::GetValue_InContainer(Container, Property);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<FString>
//{
//	using Type = typename FStrProperty;
//
//	static FString GetValue_InContainer(void* Container, Type* Property)
//	{
//		FString Ret = Property->GetPropertyValue_InContainer(Container);
//		return Ret;
//	}
//	static void SetValue_InContainer(void* Container, Type* Property, FString Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<>
//struct TPropertyTypeTrait<FName>
//{
//	using Type = typename FNameProperty;
//
//	static FName GetValue_InContainer(void* Container, Type* Property)
//	{
//		FName Ret = Property->GetPropertyValue_InContainer(Container);
//		return Ret;
//	}
//
//	static void SetValue_InContainer(void* Container, Type* Property, FName Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<>
//struct TPropertyTypeTrait<FText>
//{
//	using Type = typename FTextProperty;
//
//	static FText GetValue_InContainer(void* Container, Type* Property)
//	{
//		FText Ret = Property->GetPropertyValue_InContainer(Container);
//		return Ret;
//	}
//	static void SetValue_InContainer(void* Container, Type* Property, FText Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<typename ArrayType>
//struct TPropertyTypeTrait<TArray<ArrayType>>
//{
//	using Type = typename FArrayProperty;
//
//	static TArray<ArrayType> GetValue_InContainer(void* Container, Type* Property)
//	{
//		FScriptArrayHelper_InContainer Helper(Property, Container);
//		TArray<ArrayType> Ret;
//		for (int i = 0; i < Helper.Num(); i++)
//		{
//			using InnerPropertyType = typename TPropertyTypeTrait<ArrayType>::Type;
//			Ret.Add(TPropertyTypeTrait<ArrayType>::GetValue_InContainer(Helper.GetRawPtr(i), CastField<InnerPropertyType>(Property->Inner)));
//		}
//		return Ret;
//	}
//	static void SetValue_InContainer(void* Container, Type* Property, TArray<ArrayType> Value)
//	{
//		FScriptArrayHelper_InContainer Helper(Property, Container);
//		Helper.EmptyAndAddValues(Value.Num());
//		for (int i = 0; i < Value.Num(); i++)
//		{
//			using InnerPropertyType = typename TPropertyTypeTrait<ArrayType>::Type;
//			TPropertyTypeTrait<ArrayType>::SetValue_InContainer(Helper.GetRawPtr(i), CastField<InnerPropertyType>(Property->Inner), Value[i]);
//		}
//	}
//};
//
//template<typename KeyType, typename ValueType>
//struct TPropertyTypeTrait<TMap<KeyType, ValueType>>
//{
//	using Type = typename FMapProperty;
//
//	static TMap<KeyType, ValueType> GetValue_InContainer(void* Container, Type* Property)
//	{
//	}
//	static void SetValue_InContainer(void* Container, Type* Property, TMap<KeyType, ValueType> Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, (FScriptMap)Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<FMulticastScriptDelegate>
//{
//	using Type = typename FMulticastInlineDelegateProperty;
//	//not sure work correctly
//	static void SetValue_InContainer(void* Container, Type* Property, FMulticastScriptDelegate Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<FSparseDelegate>
//{
//	using Type = typename FMulticastSparseDelegateProperty;
//	//not sure work correctly
//	static void SetValue_InContainer(void* Container, Type* Property, FSparseDelegate Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<int8>
//{
//	using Type = typename FInt8Property;
//
//	static void SetValue_InContainer(void* Container, Type* Property, int8 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<int16>
//{
//	using Type = typename FInt16Property;
//	static void SetValue_InContainer(void* Container, Type* Property, int16 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<int32>
//{
//	using Type = typename FIntProperty;
//	static void SetValue_InContainer(void* Container, Type* Property, int32 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//
//template<>
//struct TPropertyTypeTrait<int64>
//{
//	using Type = typename FInt64Property;
//	static void SetValue_InContainer(void* Container, Type* Property, int64 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<uint8>
//{
//	using Type = typename FByteProperty;
//	static void SetValue_InContainer(void* Container, Type* Property, uint8 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<uint16>
//{
//	using Type = typename FUInt16Property;
//
//	static void SetValue_InContainer(void* Container, Type* Property, uint16 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<>
//struct TPropertyTypeTrait<uint32>
//{
//	using Type = typename FUInt32Property;
//
//	static void SetValue_InContainer(void* Container, Type* Property, uint32 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<>
//struct TPropertyTypeTrait<uint64>
//{
//	using Type = typename FUInt64Property;
//
//	static void SetValue_InContainer(void* Container, Type* Property, uint64 Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//template<>
//struct TPropertyTypeTrait<float>
//{
//	using Type = typename FFloatProperty;
//
//	static void SetValue_InContainer(void* Container, Type* Property, float Value)
//	{
//		Property->SetFloatingPointPropertyValue(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<double>
//{
//	using Type = typename FDoubleProperty;
//	static void SetValue_InContainer(void* Container, Type* Property, double Value)
//	{
//		Property->SetFloatingPointPropertyValue(Container, Value);
//	}
//};
//
//template<>
//struct TPropertyTypeTrait<bool>
//{
//	using Type = typename FBoolProperty;
//
//	static void SetValue_InContainer(void* Container, Type* Property, bool Value)
//	{
//		Property->SetPropertyValue_InContainer(Container, Value);
//	}
//};
//
//template<typename RetCppType>
//struct TValueGetter
//{
//	//Return UObject
//	template<typename RetType = RetCppType>
//	static typename TEnableIf<TIsDerivedFrom<RetType, UObject>::Value, typename RetType>::Type* GetValue(FProperty* Property, const void* ContainerAdress)
//	{
//		if (!Property)
//		{
//			return nullptr;
//		}
//
//		FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
//		if (!ensureAlwaysMsgf(ObjectProperty, TEXT("Type mismatched. Property %s is not UObject. Property type : %s"), *Property->GetNameCPP(), *Property->GetCPPType()))
//		{
//			return nullptr;
//		}
//		RetType* Casted = Cast<RetType>(ObjectProperty->GetObjectPropertyValue_InContainer(ContainerAdress));
//		ensureAlwaysMsgf(Casted, TEXT("Type mismatched. try Property %s convert to %s. actually type is %s"), *Property->GetNameCPP(), *FString(typeid(RetType).name()), *Property->GetCPPType());
//		return Casted;
//	}
//
//	//Return Value
//	template<typename RetType = RetCppType>
//	static typename TEnableIf<!TIsDerivedFrom<RetType, UObject>::Value, typename RetType>::Type GetValue(FProperty* Property, const void* ContainerAdress)
//	{
//		RetType Value;
//		FMemory::Memset(&Value, 0, sizeof(RetType));
//
//		if (!Property)
//		{
//			return Value;
//		}
//
//		using PropertyType = typename TPropertyTypeTrait<RetType>::Type;
//		PropertyType* Casted = CastField<PropertyType>(Property);
//		
//		if (!ensureAlwaysMsgf(Casted, TEXT("Type mismatched. try Property %s convert to %s. actually type is %s. return value set as 0"), *Property->GetNameCPP(), *FString(typeid(RetType).name()), *Property->GetCPPType()))
//		{
//			return Value;
//		}
//		else if(std::is_same<PropertyType, FStructProperty>::value && (Property->GetSize() != sizeof(RetType)/*타입 이름으로도 같은지 비교할 방법이 있어야 할 것 같은데..*/))
//		{
//			ensureAlwaysMsgf(false, TEXT("Struct type mismatched, try Property %s convert to %s. actually type is %s. return value set as 0"), *Property->GetNameCPP(), *FString(typeid(RetType).name()), *Property->GetCPPType());
//			return Value;
//		}
//		else if (std::is_same<PropertyType, FEnumProperty>::value && (Property->GetSize() != sizeof(RetType) || Property->GetCPPType() != StaticEnum<RetType>()->CppType))
//		{
//			ensureAlwaysMsgf(false, TEXT("Enum type mismatched, try Property %s convert to %s. actually type is %s. return value set as 0"), *Property->GetNameCPP(), *FString(typeid(RetType).name()), *Property->GetCPPType());
//			return Value;
//		}
//		Property->GetValue_InContainer(ContainerAdress, &Value);
//		return Value;
//	}
//};
//
//
//template<typename ReturnType>
//struct TReflectedValueGetter
//{
//	using RetType = typename std::conditional<TIsDerivedFrom<ReturnType, UObject>::Value, ReturnType*, ReturnType>::type;
//
//
//	//using IsUStruct
//	template<typename ContainerType> requires (!!TIsDerivedFrom<ContainerType, UObject>::Value)
//	static RetType GetValue_InContainer(FName PropertyName, ContainerType* ContainerAdress)
//	{
//		UStruct* Class = ContainerAdress->GetClass();
//		FProperty* Property = Class->FindPropertyByName(PropertyName);
//		ensureAlwaysMsgf(Property, TEXT("not exist property"));
//
//		return TValueGetter<ReturnType>::GetValue(Property, ContainerAdress);
//	}
//	
//	
//
//	template<typename ContainerType> requires (TIsDerivedFrom<ContainerType, UObject>::Value == false)
//	static RetType GetValue_InContainer(FName PropertyName, ContainerType* ContainerAdress)
//	{
//		UStruct* Class = ContainerType::StaticStruct();
//		FProperty* Property = Class->FindPropertyByName(PropertyName);
//		ensureAlwaysMsgf(Property, TEXT("not exist property"));
//		return TValueGetter<ReturnType>::GetValue(Property, ContainerAdress);
//	}
//
//	template<typename ContainerType>
//	static typename RetType GetValue_InContainer(FProperty* Property, ContainerType* ContainerAdress)
//	{
//		ensureAlwaysMsgf(Property, TEXT("not exist property"));
//		return TValueGetter<ReturnType>::GetValue(Property, ContainerAdress);
//	}
//
//
//	template<typename ContainerType> requires (!!TIsDerivedFrom<ContainerType, UObject>::Value)
//	static typename RetType GetValueByClass_InContainer(UClass* Class, ContainerType* ContainerAdress)
//	{
//		static_assert(TIsDerivedFrom<ReturnType, UObject>::Value, "RetType must be a UObject");
//
//		UClass* ContainerClass = ContainerAdress->GetClass();
//		for (TFieldIterator<FObjectProperty> It(ContainerClass); It; ++It)
//		{
//			if (It->PropertyClass->IsChildOf(Class))
//			{
//				return TValueGetter<ReturnType>::GetValue(*It, ContainerAdress);
//			}
//		}
//		return nullptr;
//	}
//
//	template<typename ContainerType> requires (TIsDerivedFrom<ContainerType, UObject>::Value == false)
//	static RetType GetValueByClass_InContainer(UClass* Class, ContainerType* ContainerAdress)
//	{
//		static_assert(TIsDerivedFrom<RetType, UObject>::Value, "RetType must be a UObject");
//
//		UStruct* Class = ContainerType::StaticStruct();
//		FProperty* FoundedProperty = nullptr;
//		for (TFieldIterator<FObjectProperty> It(Class); It; ++It)
//		{
//			if (It->PropertyClass->IsChildOf(Class))
//			{
//				return TValueGetter<ReturnType>::GetValue(*It, ContainerAdress);
//			}
//		}
//		return nullptr;
//	}
//};
//
//template<typename ReturnType>
//struct TReflectedValueSetter
//{
//	template<typename T, typename ContainerType>
//	static bool SetValue_InContainer(FName PropertyName, ContainerType* ContainerAdress, T Value)
//	{
//		if (ContainerAdress)
//		{
//			UClass* Class = ContainerType::StaticStruct();
//			if (Class)
//			{
//				FProperty* Property = Class->FindPropertyByName(PropertyName);
//				if (Property)
//				{
//					Property->SetValue_InContainer(ContainerAdress, Value);
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//
//	template<typename T, typename ContainerType> requires (!!TIsDerivedFrom<ContainerType, UObject>::Value)
//	static bool SetValue_InContainer(FName PropertyName, ContainerType* ContainerAdress, T Value)
//	{
//		if (ContainerAdress)
//		{
//			UClass* Class = ContainerAdress->GetClass();
//			if (Class)
//			{
//				FProperty* Property = Class->FindPropertyByName(PropertyName);
//				if (Property)
//				{
//					Property->SetValue_InContainer(ContainerAdress, Value);
//					return true;
//				}
//			}
//		}
//	}
//};