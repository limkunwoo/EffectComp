// Copyright Epic Games, Inc. All Rights Reserved.

#include "SampleEditorModeEditorMode.h"
#include "SampleEditorModeEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "SampleEditorModeEditorModeCommands.h"
#include "Modules/ModuleManager.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/SampleEditorModeSimpleTool.h"
#include "Tools/SampleEditorModeInteractiveTool.h"

// step 2: register a ToolBuilder in FSampleEditorModeEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "SampleEditorModeEditorMode"

const FEditorModeID USampleEditorModeEditorMode::EM_SampleEditorModeEditorModeId = TEXT("EM_SampleEditorModeEditorMode");

FString USampleEditorModeEditorMode::SimpleToolName = TEXT("SampleEditorMode_ActorInfoTool");
FString USampleEditorModeEditorMode::InteractiveToolName = TEXT("SampleEditorMode_MeasureDistanceTool");


USampleEditorModeEditorMode::USampleEditorModeEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(USampleEditorModeEditorMode::EM_SampleEditorModeEditorModeId,
		LOCTEXT("ModeName", "SampleEditorMode"),
		FSlateIcon(),
		true);
}


USampleEditorModeEditorMode::~USampleEditorModeEditorMode()
{
}


void USampleEditorModeEditorMode::ActorSelectionChangeNotify()
{
}

void USampleEditorModeEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FSampleEditorModeEditorModeCommands& SampleToolCommands = FSampleEditorModeEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<USampleEditorModeSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<USampleEditorModeInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void USampleEditorModeEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FSampleEditorModeEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> USampleEditorModeEditorMode::GetModeCommands() const
{
	return FSampleEditorModeEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
