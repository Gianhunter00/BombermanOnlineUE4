// Fill out your copyright notice in the Description page of Project Settings.


#include "BombermanMapCreationTool.h"
#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "BombermanMapCreationTool"

static const FName BombermanMapCreationName = FName("BombermanMapCreatorTool");

BombermanMapCreationTool::BombermanMapCreationTool()
{
	CheckBoxes = TArray<TSharedPtr<SCheckBox>>();
	CheckBoxes.SetNum(2);
	FileOption = TArray<TSharedPtr<SWidget>>();
	FileOption.SetNum(3);
	CreateToolOption = TArray<TSharedPtr<SWidget>>();
	CreateToolOption.SetNum(8);
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BombermanMapCreationName, FOnSpawnTab::CreateRaw(this, &BombermanMapCreationTool::OnSpawnNomad))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory());
}

BombermanMapCreationTool::~BombermanMapCreationTool()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BombermanMapCreationName);
}

TSharedRef<SDockTab> BombermanMapCreationTool::OnSpawnNomad(const FSpawnTabArgs& InTabSpawnArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("LabelFileLoad", "LOAD FROM FILE?"))
		]
	+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CheckBoxes[0], SCheckBox)
			.OnCheckStateChanged_Raw(this, &BombermanMapCreationTool::ShowOption, 0)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(-10, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("LabelCreateFromTool", "CREATE FROM TOOL?"))
		]
	+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CheckBoxes[1], SCheckBox)
			.OnCheckStateChanged_Raw(this, &BombermanMapCreationTool::ShowOption, 1)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(FileOption[0], SEditableTextBox)
			.MinDesiredWidth(100)
		.Visibility(EVisibility::Collapsed)

		]
	+ SHorizontalBox::Slot().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(FileOption[1], SButton)
			.Text(LOCTEXT("OpenFileDialog", "Open File Dialog"))
		.OnClicked_Raw(this, &BombermanMapCreationTool::OpenFileDialog)
		.Visibility(EVisibility::Collapsed)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(FileOption[2], SButton)
			.Text(LOCTEXT("CreateMap", "Create Map"))
		.OnClicked_Raw(this, &BombermanMapCreationTool::CreateMapFromFile)
		.Visibility(EVisibility::Collapsed)
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[0], STextBlock)
			.Text(LOCTEXT("LabelWidth", "Width: "))
		.Visibility(EVisibility::Collapsed)
		]
	+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[1], SSpinBox<int32>)
			.MinValue(1)
		.Value(1)
		.Visibility(EVisibility::Collapsed)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[2], STextBlock)
			.Text(LOCTEXT("LabelHeight", "Height: "))
		.Visibility(EVisibility::Collapsed)
		]
	+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[3], SSpinBox<int32>)
			.MinValue(1)
		.Value(1)
		.Visibility(EVisibility::Collapsed)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[4], STextBlock)
			.Text(LOCTEXT("BorderWall", "Are border unbreakable wall? "))
		.Visibility(EVisibility::Collapsed)
		]
	+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[5], SCheckBox)
			.IsChecked(false)
			.Visibility(EVisibility::Collapsed)
		]
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[6], SButton)
			.Text(LOCTEXT("Setup Breakable wall", "Setup Map"))
		.OnClicked_Raw(this, &BombermanMapCreationTool::OpenMapEditor)
		.Visibility(EVisibility::Collapsed)
		]
	+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8).HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(CreateToolOption[7], SButton)
			.Text(LOCTEXT("CreateMapFromTool", "Create Map"))
		  .OnClicked_Raw(this, &BombermanMapCreationTool::CreateMapFromData)
		.Visibility(EVisibility::Collapsed)
		]
			]
		];
}

FReply BombermanMapCreationTool::OpenFileDialog()
{
	if (GEngine)
	{
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
			DesktopPlatform->OpenFileDialog(nullptr, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNamesChoice);
			if (OutFileNamesChoice.Num() > 0)
			{
				Path = FPaths::ConvertRelativePathToFull(OutFileNamesChoice[0]);
				static_cast<SEditableTextBox&>(FileOption[0].ToSharedRef().Get()).SetText(FText::FromString(Path));
			}
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply BombermanMapCreationTool::CreateMapFromFile()
{
	if (!Path.IsEmpty())
	{
		if (FPaths::FileExists(Path))
		{
			if (OnPathChosenFromDialog->IsBound())
			{
				OnPathChosenFromDialog->Execute(Path);
				return FReply::Handled();
			}
		}
	}
	return FReply::Unhandled();
}

FReply BombermanMapCreationTool::CreateMapFromData()
{
	if (DataFromAdjustWindow.Num() > 0)
	{
		if (OnCreateMapTool->IsBound())
		{
			OnCreateMapTool->Execute(DataFromAdjustWindow);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Data < 0"));
	}
	return FReply::Handled();
}

FReply BombermanMapCreationTool::OpenMapEditor()
{
	int Width = static_cast<SSpinBox<int32>&>(CreateToolOption[1].ToSharedRef().Get()).GetValue();
	int Height = static_cast<SSpinBox<int32>&>(CreateToolOption[3].ToSharedRef().Get()).GetValue();
	bool bBorderBreakable = static_cast<SCheckBox&>(CreateToolOption[5].ToSharedRef().Get()).IsChecked();
	AdjustWindow = MakeShared<BombermanMapAdjustWindow>(Width, Height, CellSize, bBorderBreakable);
	AdjustWindow->OnConfirm.BindLambda([this](TArray<FString> Data) {DataFromAdjustWindow=Data;});
	return FReply::Handled();
}

void BombermanMapCreationTool::ShowOption(ECheckBoxState InState, int32 InCheckBoxID)
{
	CheckBoxes[(InCheckBoxID + 1) % 2]->SetIsChecked(false);
	ShowHideWidgets(FileOption, !InCheckBoxID && InState == ECheckBoxState::Checked);
	ShowHideWidgets(CreateToolOption, InCheckBoxID && InState == ECheckBoxState::Checked);
}

void BombermanMapCreationTool::ShowHideWidgets(TArray<TSharedPtr<SWidget>> InWidgets, bool InShow)
{
	EVisibility Visibility = InShow ? EVisibility::Visible : EVisibility::Collapsed;
	for (TSharedPtr<SWidget> Widget : InWidgets)
	{
		Widget->SetVisibility(Visibility);
	}
}

#undef LOCTEXT_NAMESPACE