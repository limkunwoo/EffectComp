// Copyright Epic Games, Inc. All Rights Reserved.

#include "SampleEditorModeEditorModeToolkit.h"
#include "SampleEditorModeEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "SampleEditorModeEditorModeToolkit"

FSampleEditorModeEditorModeToolkit::FSampleEditorModeEditorModeToolkit()
{
}

void FSampleEditorModeEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FSampleEditorModeEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FSampleEditorModeEditorModeToolkit::GetToolkitFName() const
{
	return FName("SampleEditorModeEditorMode");
}

FText FSampleEditorModeEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "SampleEditorModeEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
