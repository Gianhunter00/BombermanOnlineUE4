// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BombermanMapAdjustWindow.h"
#include "CoreMinimal.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "EnvironmentQuery/EnvQueryDebugHelpers.h"
#include "Framework/Docking/TabManager.h"
#include "LevelEditor.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

DECLARE_DELEGATE_OneParam(OnPathChosen, FString)
DECLARE_DELEGATE_OneParam(OnCreateMapTool, TArray<FString>)

class BOMBERMANJSONMAPREADER_API BombermanMapCreationTool
{
public:
	BombermanMapCreationTool();
	~BombermanMapCreationTool();

	TSharedRef<SDockTab> OnSpawnNomad(const FSpawnTabArgs& InTabSpawnArgs);

	FReply OpenFileDialog();
	FReply CreateMapFromFile();
	FReply CreateMapFromData();
	FReply OpenMapEditor();
	void ShowOption(ECheckBoxState InState, int32 InCheckBoxID);
	OnPathChosen* OnPathChosenFromDialog;
	OnCreateMapTool* OnCreateMapTool;
	void ShowHideWidgets(TArray<TSharedPtr<SWidget>> InWidgets, bool InShow);
	FString Path;

private:
	TArray<FString> DataFromAdjustWindow;
	TSharedPtr<BombermanMapAdjustWindow> AdjustWindow;
	int32 CellSize = 32;
	TSharedPtr<SGridPanel> GridPanel;
	TSharedPtr<SViewport> Viewport;
	TSharedPtr<SWindow> Window;
	const FString DialogTitle = FString("Choose the Json file to import");
	const FString DefaultPath = FPaths::GameSourceDir();
	const FString FileTypes = FString("Text Map|*.txt;");
	TArray<TSharedPtr<SCheckBox>> CheckBoxes;
	TArray<TSharedPtr<SWidget>> FileOption;
	TArray<TSharedPtr<SWidget>> CreateToolOption;
	TArray<FString> OutFileNamesChoice;
	TSharedPtr<SEditableTextBox> PathText;
};
