// Copyright Epic Games, Inc. All Rights Reserved.

#include "GenericFSMEditor.h"
#include "IAssetTools.h"
#include "Factory/FSMFactory.h"
#include "FSM/FSMBase.h"
#include "KismetCompilerModule.h"

#define LOCTEXT_NAMESPACE "FGenericFSMEditorModule"

void FGenericFSMEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	//KismetCompilerModule.GetCompilers().Add(&WidgetBlueprintCompiler);
	KismetCompilerModule.OverrideBPTypeForClass(UFSMBase::StaticClass(), UFSMBlueprint::StaticClass());
}

void FGenericFSMEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGenericFSMEditorModule, GenericFSMEditor)