// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
/**
 * 
 */

class GENERICFSMEDITOR_API FSMEditor : public FBlueprintEditor
{
public:
	FSMEditor();
	~FSMEditor();

public:
	void InitFSMEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, class UAnimBlueprint* InAnimBlueprint);
};
