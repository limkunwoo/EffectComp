// Copyright Epic Games, Inc. All Rights Reserved.

#include "SampleEditorModeModule.h"
#include "SampleEditorModeEditorModeCommands.h"
#include "MaterialVariant.h"
#include "DetailCustomize/Variant/VariantDetailCustomize.h"

#define LOCTEXT_NAMESPACE "SampleEditorModeModule"

void FSampleEditorModeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FSampleEditorModeEditorModeCommands::Register();

	FPropertyEditorModule& rPropertyModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	rPropertyModule.RegisterCustomPropertyTypeLayout(
		"MaterialVariant",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FVariantDetailCustomization<FMaterialVariant>::MakeInstance));

	rPropertyModule.RegisterCustomPropertyTypeLayout(
		"VariantTest1",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FVariantDetailCustomization<FVariantTest1>::MakeInstance));
}

void FSampleEditorModeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FSampleEditorModeEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSampleEditorModeModule, SampleEditorMode)