// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PropertyCustomizationHelpers.h"
#include "IDetailChildrenBuilder.h"
#include "VariantProxy.generated.h"

template<typename T>
struct TVariantElementTrait;

template<typename T>
struct TVariantElementDisplayTraits
{
    static FString GetDisplayName()
    {
        return TBaseStructure<T>::Get()->GetName();
    }
};

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
    virtual void Init(T& InValue)
    {
        ValueRef() = InValue;
    }

    virtual void OnStructureAdded(TSharedPtr<IPropertyHandle> StructureHandle, IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
    {
        uint32 NumChild;
        StructureHandle->GetNumChildren(NumChild);
        for (uint32 i = 0; i < NumChild; i++)
        {
            auto Handle = StructureHandle->GetChildHandle(i);
            InChildBuilder.AddProperty(Handle.ToSharedRef());
        }
    }
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

template<>
struct TVariantElementDisplayTraits<bool>
{
    static FString GetDisplayName()
    {
        return TEXT("Bool");
    }
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

template<>
struct TVariantElementDisplayTraits<int64>
{
    static FString GetDisplayName()
    {
        return TEXT("Int64");
    }
};

USTRUCT()
struct FDoubleVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<double>
{
    GENERATED_BODY()

    double& ValueRef()
    {
        return Double;
    }

    UPROPERTY(EditAnywhere)
    double Double;
};

template<>
struct TVariantElementTrait<double>
{
    using Type = FDoubleVarProxy;
};

template<>
struct TVariantElementDisplayTraits<double>
{
    static FString GetDisplayName()
    {
        return TEXT("Double");
    }
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
template<>
struct TVariantElementDisplayTraits<FString>
{
    static FString GetDisplayName()
    {
        return TEXT("String");
    }
};

USTRUCT()
struct FSoftObjectPtrVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<TSoftObjectPtr<UObject>>
{
    GENERATED_BODY()

    TSoftObjectPtr<UObject>& ValueRef()
    {
        return Object;
    }
    virtual void Init(TSoftObjectPtr<UObject>& InValue) override
    {
        Object = InValue;
        Class = UObject::StaticClass();
    }
    template<typename T> requires (!std::is_same_v<UObject, T>)
    void Init(TSoftObjectPtr<T>& InValue)
    {
        Class = T::StaticClass();
        Object = InValue;
    }
    virtual void OnStructureAdded(TSharedPtr<IPropertyHandle> StructureHandle, IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
    {
        InChildBuilder.AddCustomRow(FText::FromString(TEXT("Object")))
        .NameContent()
        [
            SNew(STextBlock).Text(FText::FromString(Class->GetName()))
        ]
        .ValueContent()
        [
            SNew(SObjectPropertyEntryBox)
            .AllowedClass(Class)
            .PropertyHandle(StructureHandle->GetChildHandle(0))
            .ThumbnailPool(InCustomizationUtils.GetThumbnailPool())
        ];
    }

    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UObject> Object;
    UPROPERTY()
     
    UClass* Class;
};

template<typename T>
struct TVariantElementTrait<TSoftObjectPtr<T>>
{
    using Type = FSoftObjectPtrVarProxy;

    static TSoftObjectPtr<T> Conversion(TSoftObjectPtr<UObject> Value)
    {
        return TSoftObjectPtr<T>(Value.ToSoftObjectPath());
    }
};

template<typename T>
struct TVariantElementDisplayTraits<TSoftObjectPtr<T>>
{
    static FString GetDisplayName()
    {
        return T::StaticClass()->GetName();
    }
};
