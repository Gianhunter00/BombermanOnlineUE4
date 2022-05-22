// Fill out your copyright notice in the Description page of Project Settings.


#include "BombermanMapAdjustWindow.h"

#define LOCTEXT_NAMESPACE "BombermanMapAdjustWindow"

BombermanMapAdjustWindow::BombermanMapAdjustWindow()
{
}

BombermanMapAdjustWindow::BombermanMapAdjustWindow(int32 InWidth, int32 InHeight, int32 InSize, bool AreBordersUnbreakable) : BombermanMapAdjustWindow()
{
	MapWidth = InWidth;
	MapHeight = InHeight;
	ButtonSize = InSize;
	bBorderUnbreakable = AreBordersUnbreakable;
	Data = TArray<FString>();
	Cell.SetNum(MapWidth * MapHeight);
	ButtonsColorOption.Add(FLinearColor::Gray);
	ButtonsColorOption.Add(FLinearColor::Green);
	ButtonsColorOption.Add(FLinearColor::Red);
	Window = SNew(SWindow)
		.Title(FText::FromString("Adjust Map Tool"))
		.ClientSize(FVector2D(MapWidth * ButtonSize + 50, MapHeight * ButtonSize + 100))
		.SizingRule(ESizingRule::FixedSize);

	FVector2D CanvasSize = Window->GetClientSizeInScreen();
	CanvasSize.X -= 50;
	CanvasSize.Y -= 50;
	Viewport = SNew(SViewport)
		[
			SNew(SCanvas)
			+ SCanvas::Slot().Position(FVector2D(0, 0)).Size(Window->GetClientSizeInScreen())
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().HAlign(HAlign_Left).AutoHeight().MaxHeight(Window->GetClientSizeInScreen().Y - 100)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot().MaxWidth(Window->GetClientSizeInScreen().X - 100)
					[
						SNew(SCanvas)
						+ SCanvas::Slot().Expose(CanvasData).Position(FVector2D(0, 0)).Size(FVector2D(Window->GetClientSizeInScreen().X - 50, Window->GetClientSizeInScreen().Y - 50))
						[
							CreateGridPanel()
						]
					]
				]
				+ SVerticalBox::Slot().HAlign(EHorizontalAlignment::HAlign_Right)
				[
					SAssignNew(VerticalSlider, SSlider)
					.Orientation(EOrientation::Orient_Vertical)
					.OnValueChanged_Raw(this, &BombermanMapAdjustWindow::SetCanvasPosition, true)
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().VAlign(EVerticalAlignment::VAlign_Bottom)
					[
						SAssignNew(HorizontalSlider, SSlider)
						.OnValueChanged_Raw(this, &BombermanMapAdjustWindow::SetCanvasPosition, false)
					]
				]
				+ SVerticalBox::Slot().HAlign(EHorizontalAlignment::HAlign_Center).AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().VAlign(EVerticalAlignment::VAlign_Bottom)
					[
						SNew(SButton)
						.Text(LOCTEXT("SetupMap", "Confirm"))
						.OnClicked_Raw(this, &BombermanMapAdjustWindow::PopulateData)
					]
				]
			]
		];
	Window->SetContent(Viewport.ToSharedRef());
	CloseDelegateHandle = FSlateApplication::Get().OnWindowBeingDestroyed().AddLambda([this](const SWindow& InWindow)
		{
			if (Window.Get() == &InWindow)
			{
				HorizontalSlider.Reset();
				VerticalSlider.Reset();
				GridPanel.Reset();
				Viewport.Reset();
			}
		});
	VerticalSlider->SetMinAndMaxValues(0, FMath::Clamp((MapHeight * ButtonSize) - CanvasData->SizeAttr.Get().Y, 0.f, INFINITY));
	VerticalSlider->SetValue(VerticalSlider->GetMaxValue());

	HorizontalSlider->SetMinAndMaxValues(0, FMath::Clamp((MapWidth * ButtonSize) - CanvasData->SizeAttr.Get().X, 0.f, INFINITY));
	FSlateApplication::Get().AddWindow(Window.ToSharedRef());
}

BombermanMapAdjustWindow::~BombermanMapAdjustWindow()
{
	if (FSlateApplication::IsInitialized())
	{
		if (CloseDelegateHandle.IsValid())
		{
			FSlateApplication::Get().OnWindowBeingDestroyed().Remove(CloseDelegateHandle);
		}

		if (Window.IsValid())
		{
			Window->RequestDestroyWindow();
			Window.Reset();
		}
	}
}

TSharedRef<SGridPanel> BombermanMapAdjustWindow::CreateGridPanel()
{
	if (bBorderUnbreakable)
	{
		CreateGridPanelBordersUnbreakable();
	}
	else
	{
		CreateGridPanelNoBordersUnbreakable();
	}
	return GridPanel.ToSharedRef();
}

void BombermanMapAdjustWindow::CreateGridPanelNoBordersUnbreakable()
{
	GridPanel.Reset();
	GridPanel = SNew(SGridPanel).Clipping(EWidgetClipping::ClipToBounds);
	for (int32 Height = 0; Height < MapHeight; Height++)
	{
		for (int32 Width = 0; Width < MapWidth; Width++)
		{
			GridPanel->AddSlot(Width, Height).AttachWidget(
			SNew(SBox).WidthOverride(ButtonSize).HeightOverride(ButtonSize)
			[ 
				 SAssignNew(Cell[Height * MapWidth + Width], SButton)
				.ForegroundColor(ButtonsColorOption[0])
				.ButtonColorAndOpacity(ButtonsColorOption[0])
				.OnClicked_Raw(this, &BombermanMapAdjustWindow::ChangeButtonColor, Height * MapWidth + Width)
			]);
		}
	}
}

void BombermanMapAdjustWindow::CreateGridPanelBordersUnbreakable()
{
	GridPanel.Reset();
	GridPanel = SNew(SGridPanel).Clipping(EWidgetClipping::ClipToBounds);
	int32 ColorIndex = 0;
	for (int32 Height = 0; Height < MapHeight; Height++)
	{
		for (int32 Width = 0; Width < MapWidth; Width++)
		{
			ColorIndex = (Height == 0 || Height == (MapHeight - 1) || Width == 0 || Width == (MapWidth - 1)) ? 2 : 0;
			GridPanel->AddSlot(Width, Height).AttachWidget(
				SNew(SBox).WidthOverride(ButtonSize).HeightOverride(ButtonSize)
				[
					SAssignNew(Cell[Height * MapWidth + Width], SButton)
					.ForegroundColor(ButtonsColorOption[ColorIndex])
					.ButtonColorAndOpacity(ButtonsColorOption[ColorIndex])
					.OnClicked_Raw(this, &BombermanMapAdjustWindow::ChangeButtonColor, Height * MapWidth + Width)
				]);
		}
	}
}

FReply BombermanMapAdjustWindow::ChangeButtonColor(int32 InCellIndex)
{
	TSharedPtr<SButton> InButtonClicked = Cell[InCellIndex];
	for (int32 Index = 0; Index < ButtonsColorOption.Num(); Index++)
	{
		if (InButtonClicked->GetForegroundColor().GetSpecifiedColor().Equals(ButtonsColorOption[Index]))
		{
			InButtonClicked->SetForegroundColor(ButtonsColorOption[(Index + 1) % 3]);
			InButtonClicked->SetBorderBackgroundColor(ButtonsColorOption[(Index + 1) % 3]);
			return FReply::Handled();
		}
	}
	return FReply::Handled();
}

FReply BombermanMapAdjustWindow::PopulateData()
{
	FString Temp = "";
	for (int32 Height = 0; Height < MapHeight; Height++)
	{
		for (int32 Width = 0; Width < MapWidth; Width++)
		{
			for (int32 ColorIndex = 0; ColorIndex < ButtonsColorOption.Num(); ColorIndex++)
			{
				if (Cell[Height * MapWidth + Width]->GetForegroundColor().GetSpecifiedColor().Equals(ButtonsColorOption[ColorIndex]))
				{
					FString Current;
					if (Width + 1 == MapWidth)
					{
						Current = FString::Printf(TEXT("%d"), ColorIndex);
					}
					else
					{
						Current = FString::Printf(TEXT("%d,"), ColorIndex);
					}
					Temp.Append(Current);
				}
			}
		}
		Data.Add(Temp);
		Temp.Empty();
	}
	if (OnConfirm.IsBound())
	{
		OnConfirm.Execute(Data);
	}
	Data.Reset();
	if (FSlateApplication::IsInitialized())
	{
		if (CloseDelegateHandle.IsValid())
		{
			FSlateApplication::Get().OnWindowBeingDestroyed().Remove(CloseDelegateHandle);
		}

		if (Window.IsValid())
		{
			Window->RequestDestroyWindow();
		}
	}
	return FReply::Handled();
}

void BombermanMapAdjustWindow::SetCanvasPosition(float InValue, bool bVertical)
{
	FVector2D CurrentPosition;
	FVector2D Size = CanvasData->SizeAttr.Get();
	if (bVertical)
	{
		CurrentPosition = CanvasData->PositionAttr.Get();
		CurrentPosition.Y = -(VerticalSlider->GetMaxValue() - InValue);
		Size.Y = (Window->GetClientSizeInScreen().Y - 50) + (VerticalSlider->GetMaxValue() - InValue);
	}
	else
	{
		CurrentPosition = CanvasData->PositionAttr.Get();
		CurrentPosition.X = -InValue;
		Size.X = (Window->GetClientSizeInScreen().X - 50) + InValue;
	}
	CanvasData->Position(CurrentPosition);
	CanvasData->Size(Size);
}

#undef LOCTEXT_NAMESPACE