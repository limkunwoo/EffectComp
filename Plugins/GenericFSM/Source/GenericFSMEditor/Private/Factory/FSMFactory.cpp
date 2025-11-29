// Fill out your copyright notice in the Description page of Project Settings.


#include "Factory/FSMFactory.h"
#include "FSM/FSMBase.h"
#include "ContentBrowserMenuContexts.h"
#include "ToolMenuSection.h"

UFSMFactory::UFSMFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UFSMBlueprint::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}
UObject* UFSMFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return Super::FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn); //NewObject<UFSMBlueprint>(InParent, InClass, InName, Flags);
}

bool UFSMFactory::ShouldShowInNewMenu() const
{
    return true;
}

namespace MenuExtension_FSMBlueprint
 {
    static void ExecuteActionForYourType(const FToolMenuContext& InContext)
     {
        const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext);
        
        for (UFSMBlueprint* LoadedInstance : Context->LoadSelectedObjects<UFSMBlueprint>())
        {
        
        }
    }
    static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, [] {
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
        {
            FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
            UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UFSMBlueprint::StaticClass());
            
            FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
            Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
                { 
                    const TAttribute<FText> Label = FText::FromString(TEXT("YOUR_ASSET_TYPE_ExecuteActionForYourType"));
                    const TAttribute<FText> ToolTip = FText::FromString(TEXT("This will execute the action for your type."));
                    const FSlateIcon Icon = FSlateIcon();


                    FToolUIAction UIAction;
                    UIAction.ExecuteAction = FToolMenuExecuteAction::CreateStatic(&ExecuteActionForYourType);
                    InSection.AddMenuEntry("YOUR_ASSET_TYPE_ExecuteActionForYourType", Label, ToolTip, Icon, UIAction);
                }));
        }));
    });
}

TSoftClassPtr<UObject> UAssetDefinition_FSMBlueprint::GetAssetClass() const
{
	return UFSMBlueprint::StaticClass();
}

UFactory* UAssetDefinition_FSMBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
    UFSMFactory* FSMFactory = NewObject<UFSMFactory>();
    FSMFactory->ParentClass = TSubclassOf<UFSMBase>(*InBlueprint->GeneratedClass);
    return FSMFactory;
}
