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
#include "VariantProxy.h"
#include "SSearchableComboBox.h"
/**
 * 
 */

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

template<typename... Ts>
TArray<TSharedPtr<FString>> GetTypeOptions(TVariant<Ts...>& Variant)
{
    TArray<TSharedPtr<FString>> Result = { MakeShared<FString>(TVariantElementDisplayTraits<Ts>::GetDisplayName())... };
    return Result;
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

        FVariantType* CurrValue = nullptr;
        GetValueRef(CurrValue);
        
        TypeOptions = GetTypeOptions(CurrValue->Variant);
        TSharedPtr<FString> InitiallySellectedItem = TypeOptions[CurrValue->GetIndex()];

        InHeaderRow
        .NameContent()
        [
            InPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        [
            SAssignNew(TypeCombobox, SSearchableComboBox)
            .Content()
            [
                SNew(STextBlock)
                .Text_Lambda([this]() 
                    {
                        FVariantType* CurrValue = nullptr;
                        GetValueRef(CurrValue);

                        if (!CurrValue) return FText::FromString(TEXT("Error"));

                        int32 Index = CurrValue->GetIndex();
                        return FText::FromString(*TypeOptions[Index]);
                    })
            ]
            .OptionsSource(&TypeOptions)
            .OnGenerateWidget(this, &FVariantDetailCustomization::OnGenerateComboWidget)
            .OnSelectionChanged(this, &FVariantDetailCustomization::OnSelectionChangedInternal)
            //.OnComboBoxOpening(InArgs._OnComboBoxOpening)
            .InitiallySelectedItem(InitiallySellectedItem)
            .SearchVisibility(EVisibility::Collapsed)
        ];
	}
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle, IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils) override
	{
        LayoutBuilder = &InChildBuilder.GetParentCategory().GetParentLayout();


        FVariantType* CurrValue = nullptr;
        GetValueRef(CurrValue);
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
                ProxyRawPtr->Init(Value);
                Proxy = MoveTemp(ProxyPtr);
                
                auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(ProxyType::StaticStruct(), (uint8*)ProxyRawPtr)));
                Row->Visibility(EVisibility::Collapsed);
                TSharedPtr<IPropertyHandle> ChildStructHandle = Row->GetPropertyHandle();
                ChildStructHandle->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateLambda([this, ProxyRawPtr]()
                {
                    if constexpr (requires() { TVariantElementTrait<ValueType>::Conversion(Value); })
                    {
                        OnValueChanged(TVariantElementTrait<ValueType>::Conversion(ProxyRawPtr->ValueRef()));
                    }
                    else
                    {
                        OnValueChanged(ProxyRawPtr->ValueRef());
                    }
                }));
                ProxyRawPtr->OnStructureAdded(ChildStructHandle, InChildBuilder, InCustomizationUtils);
            }
            
        });
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

    TSharedRef<SWidget> OnGenerateComboWidget(TSharedPtr<FString> InComboString)
    {
        return
            SNew(STextBlock)
            .Text(FText::FromString(*InComboString));
    }

    void OnSelectionChangedInternal(TSharedPtr<FString> InSelectedItem, ESelectInfo::Type SelectInfo)
    {
        int32 Index = TypeOptions.IndexOfByKey(InSelectedItem);
        if (Index != INDEX_NONE)
        {
            FVariantType* CurrValue;
            GetValueRef(CurrValue);
            if (!CurrValue)
            {
                LayoutBuilder->ForceRefreshDetails();
            }

            EmplaceImpl(Index, CurrValue->Variant);
            LayoutBuilder->ForceRefreshDetails();
        }
        else
        {
            TypeCombobox->ClearSelection();
        }
    }

    TSharedPtr<IPropertyHandle> VariantStructPropertyHandle;
    IDetailLayoutBuilder* LayoutBuilder;
    TUniquePtr<FVariantElementProxy> Proxy;
    TArray<TSharedPtr<FString>> TypeOptions;
    TSharedPtr<SSearchableComboBox> TypeCombobox;
};

