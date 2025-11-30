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

template<typename T, typename TDerived>
struct TVariantElementProxyImpl
{
    virtual ~TVariantElementProxyImpl() {}
    virtual T& ValueRef() = 0;
    virtual void Init(T& InValue, TAttribute<EVisibility> InVisibility)
    {
        ValueRef() = InValue;
        Visibility = InVisibility;
    }
    
    template<typename T>
    T& GetValueAs()
    {
        return ValueRef();
    }
    
    virtual IDetailPropertyRow* Customize(IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
    {
        TDerived& DerivedRef = *static_cast<TDerived*>(this);
        auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(TDerived::StaticStruct(), (uint8*)&DerivedRef)));
        Row->Visibility(EVisibility::Collapsed);
        auto StructureHandle = Row->GetPropertyHandle();
        uint32 NumChild;
        StructureHandle->GetNumChildren(NumChild);
        for (uint32 i = 0; i < NumChild; i++)
        {
            auto Handle = StructureHandle->GetChildHandle(i);
            InChildBuilder.AddProperty(Handle.ToSharedRef()).Visibility(Visibility);
        }
        return Row;
    }
    TAttribute<EVisibility> Visibility;
};

USTRUCT()
struct FDefaultVarProxy : public FVariantElementProxy
{
    GENERATED_BODY()

    ~FDefaultVarProxy()
    {
    }

    struct FStorage
    {
        virtual ~FStorage()
        {
        }

        virtual uint8* GetBuffer() const = 0;
    };

    template<typename T>
    struct TStorage : public FStorage
    {
        virtual ~TStorage() {}
        virtual uint8* GetBuffer() const override
        {
            return (uint8*)&Data;
        }
        T Data;
    };

    template<typename T>
    void Init(T& InValue, TAttribute<EVisibility> InVisibility)
    {
        ScriptStruct = TBaseStructure<T>::Get();
        Buffer = MakeShared<TStorage<T>>();
        GetValueAs<T>() = InValue;
        Visibility = InVisibility;
    }
    
    IDetailPropertyRow* Customize(IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
    {
        auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(ScriptStruct, Buffer->GetBuffer())));
        Row->Visibility(Visibility);
        return Row;
    }

    template<typename T>
    T& GetValueAs()
    {
        check(ScriptStruct == TBaseStructure<T>::Get());
        return *(T*)Buffer->GetBuffer();
    }

    UScriptStruct* ScriptStruct;
    TSharedPtr<FStorage> Buffer;
    TAttribute<EVisibility> Visibility;
};

template<typename T>
concept HasBaseStruct = requires() { TBaseStructure<T>::Get(); };

template<HasBaseStruct Struct>
struct TVariantElementTrait<Struct>
{
    using Type = FDefaultVarProxy;
};

USTRUCT()
struct FBoolVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<bool, FBoolVarProxy>
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
struct FInt64VarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<int64, FInt64VarProxy>
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
struct FDoubleVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<double, FDoubleVarProxy>
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
struct FStringVarProxy : public FVariantElementProxy, public TVariantElementProxyImpl<FString, FStringVarProxy>
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
struct FSoftObjectPtrVarProxy : public FVariantElementProxy
{
    GENERATED_BODY()

    TSoftObjectPtr<UObject>& ValueRef()
    {
        return Object;
    }

    template<typename T>
    void Init(TSoftObjectPtr<T>& InValue, TAttribute<EVisibility> InVisibility)
    {
        Class = T::StaticClass();
        Object = InValue;
        Visibility = InVisibility;
    }
    
    virtual IDetailPropertyRow* Customize(IDetailChildrenBuilder& InChildBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
    {
        auto Row = InChildBuilder.AddExternalStructure(MakeShareable(new FStructOnScope(FSoftObjectPtrVarProxy::StaticStruct(), (uint8*)this)));
        Row->Visibility(EVisibility::Collapsed);
        auto StructureHandle = Row->GetPropertyHandle();
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
        ]
        .Visibility(Visibility);
        return Row;
    }

    template<typename T>
    T GetValueAs()
    {
        return T(Object.ToSoftObjectPath());
    }

    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UObject> Object;
    UPROPERTY()
     
    UClass* Class;
    TAttribute<EVisibility> Visibility;
};

template<typename T>
struct TVariantElementTrait<TSoftObjectPtr<T>>
{
    using Type = FSoftObjectPtrVarProxy;
};

template<typename T>
struct TVariantElementDisplayTraits<TSoftObjectPtr<T>>
{
    static FString GetDisplayName()
    {
        return T::StaticClass()->GetName();
    }
};
