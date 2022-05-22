// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SCanvas.h"
#include "Widgets/SViewport.h"
/**
 * 
 */
DECLARE_DELEGATE_OneParam(OnConfirmMapAdjustWindow, TArray<FString>)
class BOMBERMANJSONMAPREADER_API BombermanMapAdjustWindow
{
public:
	BombermanMapAdjustWindow();
	BombermanMapAdjustWindow(int32 InWidth, int32 InHeight, int32 InSize, bool bBorderUnbreakable);
	~BombermanMapAdjustWindow();

	FDelegateHandle CloseDelegateHandle;
	TSharedRef<SGridPanel> CreateGridPanel();
	void CreateGridPanelNoBordersUnbreakable();
	void CreateGridPanelBordersUnbreakable();
	FReply ChangeButtonColor(int32 InCellIndex);
	FReply PopulateData();
	void SetCanvasPosition(float InValue, bool bVertical);
	OnConfirmMapAdjustWindow OnConfirm;

private:
	int32 MapWidth;
	int32 MapHeight;
	int32 ButtonSize;
	bool bBorderUnbreakable;
	TArray<FString> Data;
	TArray<FLinearColor> ButtonsColorOption;
	TArray<TSharedPtr<SButton>> Cell;
	SCanvas::FSlot* CanvasData;
	TSharedPtr<SGridPanel> GridPanel;
	TSharedPtr<SSlider> VerticalSlider;
	TSharedPtr<SSlider> HorizontalSlider;
	TSharedPtr<SViewport> Viewport;
	TSharedPtr<SWindow> Window;
};
