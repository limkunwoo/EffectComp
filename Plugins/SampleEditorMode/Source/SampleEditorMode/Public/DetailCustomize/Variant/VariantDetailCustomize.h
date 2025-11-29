// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "IPropertyTypeCustomization.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "VariantDetailCustomize.generated.h"
/**
 * 
 */

template<typename T>
struct TVariantElementTrait;

USTRUCT()
struct FVariantElementProxy
{
    virtual ~FVariantElementProxy() {}
    GENERATED_BODY()
};

template<typename T>
struct TVariantElementProxyImpl
{
    virtual ~TVariantElementProxyImpl() {}
    virtual T& ValueRef() = 0;
};
USTRUCT()
struct FBoolVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<bool>
{
    GENERATED_BODY()

    bool& ValueRef()
    {
        return Bool;
    }
    UPROPERTY(EditAnywhere)
    bool Bool;
};

template<>
struct TVariantElementTrait<bool>
{
    using Type = FBoolVarProxy;
};

USTRUCT()
struct FInt64VarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<int64>
{
    GENERATED_BODY()

    int64& ValueRef()
    {
        return Int64;
    }

    UPROPERTY(EditAnywhere)
    int64 Int64;
};

template<>
struct TVariantElementTrait<int64>
{
    using Type = FInt64VarProxy;
};

USTRUCT()
struct FStringVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<FString>
{
    GENERATED_BODY()

    FString& ValueRef()
    {
        return String;
    }

    UPROPERTY(EditAnywhere)
    FString String;
};

template<>
struct TVariantElementTrait<FString>
{
    using Type = FStringVarProxy;
};

template<typename T, typename VariantType>
void EmplaceVariant(VariantType& Variant)
{
    Variant.Emplace<T>();
}

template<typename... Ts>
void EmplaceImpl(SIZE_T Index, TVariant<Ts...>& Variant)
{
    using VariantType = TVariant<Ts...>;
    using FnInvoker = void(*)(VariantType&);
    static constexpr FnInvoker Invokers[] = { &EmplaceVariant<Ts, VariantType>... };
    Invokers[Index](Variant);
}

template<typename FVariantType>
class FVariantDetailCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FVariantDetailCustomization>();
	}

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle, FDetailWidgetRow& InHeaderRow, IPropertyTypeCustomizationUtils& InCustomizationUtils) override
    {
        VariantStructPropertyHandle = InPropertyHandle;
        TSharedPtr<IPropertyHandle> TypeIndexHandle = InPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVariantType, Index));

        FVariantType* CurrValue = nullptr;
        GetValueRef(CurrValue);
        if (CurrValue)
        {
            CurrValue->Index = (FVariantType::EVariantIndex)(CurrValue->Variant.GetIndex());
        }
        
        InHeaderRow
        .NameContent()
        [
            InPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        [
            TypeIndexHandle->CreatePropertyValueWidget()
        ];
	}
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle, IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils) override
	{
        LayoutBuilder = &InChildBuilder.GetParentCategory().GetParentLayout();

        TSharedPtr<IPropertyHandle> TypeIndexHandle = InPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVariantType, Index));
        TypeIndexHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([this]()
            {
                FVariantType* CurrValue;
                GetValueRef(CurrValue);
                if (!CurrValue)
                {
                    LayoutBuilder->ForceRefreshDetails();
                }

                EmplaceImpl((int32)CurrValue->Index, CurrValue->Variant);
                LayoutBuilder->ForceRefreshDetails();
            }));

        FVariantType* CurrValue = nullptr;
        GetValueRef(CurrValue);
        TSharedPtr<IPropertyHandle> ChildStructHandle;
        CurrValue->Visit([&](auto& Value)
        {
            using ValueType = std::remove_cvref_t<decltype(Value)>;
            if constexpr (requires() { TBaseStructure<ValueType>::Get(); })
            {
                auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(TBaseStructure<ValueType>::Get(), (uint8*)&Value)));
            }
            else
            {
                using ProxyType = TVariantElementTrait<ValueType>::Type;

                TUniquePtr<ProxyType> ProxyPtr = MakeUnique<ProxyType>();
                ProxyType* ProxyRawPtr = ProxyPtr.Get();
                Proxy = MoveTemp(ProxyPtr);

                ProxyRawPtr->ValueRef() = Value;
                auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(ProxyType::StaticStruct(), (uint8*)ProxyRawPtr)));
                Row->Visibility(EVisibility::Collapsed);
                ChildStructHandle = Row->GetPropertyHandle();
                ChildStructHandle->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateLambda([this, ProxyRawPtr]()
                    {
                        OnValueChanged(ProxyRawPtr->ValueRef());
                    }));
            }
            
        });

        uint32 NumChild;
        if (ChildStructHandle && ChildStructHandle->GetNumChildren(NumChild))
        {
            for (uint32 i = 0; i < NumChild; i++)
            {
                auto Handle = ChildStructHandle->GetChildHandle(i);
                InChildBuilder.AddProperty(Handle.ToSharedRef());
            }
        }
	}

    FPropertyAccess::Result GetValueRef(FVariantType*& OutValue)
	{
        if (GIsSavingPackage || IsGarbageCollecting())
        {
            return FPropertyAccess::Fail;
        }

        FPropertyAccess::Result Result = FPropertyAccess::Fail;

        if (VariantStructPropertyHandle.IsValid() && VariantStructPropertyHandle->IsValidHandle())
        {
            TArray<void*> RawData;
            VariantStructPropertyHandle->AccessRawData(RawData);

            if (RawData.Num() > 1)
            {
                Result = FPropertyAccess::MultipleValues;
            }

            if (RawData.Num() == 1 && RawData[0] != nullptr)
            {
                OutValue = reinterpret_cast<FVariantType*>(RawData[0]);
                Result = FPropertyAccess::Success;
            }
        }

        return Result;
	}
	template<typename T>
	void OnValueChanged(T&& InValue)
	{
        FVariantType* CurrValue = nullptr;
        GetValueRef(CurrValue);
        if (!CurrValue)
        {
            return;
        }
        
        if (CurrValue->Variant.IndexOfType<std::remove_cvref_t<T>>() != CurrValue->Variant.GetIndex())
        {
            LayoutBuilder->ForceRefreshDetails();
        }
        FScopedTransaction Transaction(FText::FromString(TEXT("Set Struct Value")));
        VariantStructPropertyHandle->NotifyPreChange();
        *CurrValue = Forward<T>(InValue);
        VariantStructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
	}


	TSharedPtr<IPropertyHandle> VariantStructPropertyHandle;
    IDetailLayoutBuilder* LayoutBuilder;
    TUniquePtr<FVariantElementProxy> Proxy;
};

