#include "SolutionSelector.h"

#include "Misc/Guid.h"  // FGuid
#include "EditorStyleSet.h" // FEditorStyle
#include "Widgets/SBoxPanel.h"  // SVerticalBox
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Layout/SSpacer.h" // SSpacer

#include "Engine.h" // GEngine

SolutionSelector::SolutionSelector()
    : MainLayout(SNew(SVerticalBox))
    , SolutionWidgetContainer(SNew(SVerticalBox))
    , ToolBarContainer(SNew(SHorizontalBox))
    , CurrentSelectedSolutionIndex(-1)
{
    Initialize();
}

TSharedRef<SWidget> SolutionSelector::Self()
{
    return MainLayout;
}

void SolutionSelector::AddSolution(FString SolutionName, FString SolutionToolTip)
{
    // The new solution index
    const int SolutionIndex = SlateWidgetRef.Num();

    TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
        .Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
        .OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState) {
            // Set box check state
            int Index = InferClickedButtonIndex(CheckState);
            UpdateClickButtonState(Index);
        })
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(SSpacer)
                .Size(FVector2D(1, 30))
            ]
            + SOverlay::Slot()
            .Padding(FMargin(6, 0, 15, 0))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(SolutionName))
                .ToolTipText(FText::FromString(SolutionToolTip))
            ]

            + SOverlay::Slot()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Left)
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("PlacementBrowser.ActiveTabBar"))
            ]
        ];

    // update slate UI
    SolutionWidgetContainer->InsertSlot(SolutionIndex).AutoHeight()[CheckBox];

    // add ref to container
    SlateWidgetRef.Push(CheckBox);
    ensure(SlateWidgetRef.Num() == SolutionWidgetContainer->NumSlots());

    // Callback
    ensure(CB_Append);
    CB_Append(SolutionIndex);
}

void SolutionSelector::Initialize()
{
    MainLayout->AddSlot().AutoHeight()[ToolBarContainer];
    MainLayout->AddSlot().AutoHeight()[SolutionWidgetContainer];

    TSharedRef<SWidget> BtnAdd = SNew(SButton)
        .Text(FText::FromString("ADD"))
        .OnClicked_Lambda([this]() -> FReply {
            AddSolution("FUCK", "SHIT");
            return FReply::Handled();
        });

    TSharedRef<SWidget> BtnRemove = SNew(SButton)
        .Text(FText::FromString("REMOVE"))
        .OnClicked_Lambda([this]() -> FReply {
            // AddSolution("FUCK", "SHIT");
            if (CurrentSelectedSolutionIndex >= 0) {
                auto ref = SlateWidgetRef[CurrentSelectedSolutionIndex];
                auto idx = SolutionWidgetContainer->RemoveSlot(ref);
                ensure(idx >= 0);
                SlateWidgetRef.RemoveAt(CurrentSelectedSolutionIndex);
            }
            UpdateClickButtonState(CurrentSelectedSolutionIndex - 1);
            return FReply::Handled();
        });

    TSharedRef<SWidget> BtnRename = SNew(SButton)
        .Text(FText::FromString("RENAME"))
        .OnClicked_Lambda([&]() -> FReply {
            AddSolution("FUCK", "SHIT");
            return FReply::Handled();
        });

    ToolBarContainer->AddSlot().AutoWidth()[BtnAdd];
    ToolBarContainer->AddSlot().AutoWidth()[BtnRemove];
    ToolBarContainer->AddSlot().AutoWidth()[BtnRename];
}

void SolutionSelector::UpdateClickButtonState(int CheckedIndex)
{
    // clear all box check state
    for (auto Widget : SlateWidgetRef) {
        Widget->SetIsChecked(ECheckBoxState::Unchecked);
    }
    // toggle checked box
    if (CheckedIndex >= 0) {
        ensure(CheckedIndex < SlateWidgetRef.Num());
        SlateWidgetRef[CheckedIndex]->SetIsChecked(ECheckBoxState::Checked);
    }

    // callback
    ensure(CB_Active);
    if (CurrentSelectedSolutionIndex != CheckedIndex) {
        CB_Active(CheckedIndex);
    }

    // Set current selected
    CurrentSelectedSolutionIndex = CheckedIndex;
}

int SolutionSelector::InferClickedButtonIndex(ECheckBoxState CheckState)
{
    if (CheckState == ECheckBoxState::Unchecked) {
        return CurrentSelectedSolutionIndex;
    }
    else if (CheckState == ECheckBoxState::Checked) {
        TArray<int> CheckedIndex;
        for (int i = 0; i < SlateWidgetRef.Num(); ++i) {
            auto Widget = SlateWidgetRef[i];
            bool IsChecked = Widget->IsChecked();
            if (IsChecked) {
                CheckedIndex.Push(i);
            }
            //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
            //    FString::Printf(TEXT("Index %d check status %d"), i, int(IsChecked)));
        }

        if (CurrentSelectedSolutionIndex >= 0) {
            ensure(CheckedIndex.Num() == 2);
            return CheckedIndex[0] != CurrentSelectedSolutionIndex
                ? CheckedIndex[0] : CheckedIndex[1];
        }
        else {
            ensure(CheckedIndex.Num() == 1);
            return CheckedIndex[0];
        }
    }
    // ECheckBoxState::Undetermined
    else {
        // TODO: why
        ensure(false);
        return -1;
    }
}
