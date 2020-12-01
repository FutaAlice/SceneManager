#include "SolutionSelector.h"

#include "Misc/Guid.h"  // FGuid
#include "EditorStyleSet.h" // FEditorStyle
#include "Widgets/SBoxPanel.h"  // SVerticalBox
#include "Widgets/SWindow.h"    // SWindow
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Input/SEditableText.h"    // SEditableText
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Layout/SSpacer.h" // SSpacer
#include "Editor/EditorEngine.h"    // EditorAddModalWindow
#include "Editor.h" // GEditor

#include "Engine.h" // GEngine

FSolutionSelector::FSolutionSelector()
    : MainLayout(SNew(SVerticalBox))
    , SolutionWidgetContainer(SNew(SVerticalBox))
    , ToolBarContainer(SNew(SHorizontalBox))
    , CurrentSelectedSolutionIndex(-1)
{
    Initialize();
}

TSharedRef<SWidget> FSolutionSelector::Self()
{
    return MainLayout;
}

void FSolutionSelector::AddSolution()
{
    FString SolutionName = FString::Printf(TEXT("Solution %d"), Num() + 1);
    FString SolutionToolTip = FString::Printf(TEXT("The %d-th Solution"), Num() + 1);
    AddSolution(SolutionName, SolutionToolTip);
}

void FSolutionSelector::AddSolution(FString SolutionName, FString SolutionToolTip)
{
    TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
        .Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
        .OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState) {
            // Set box check state
            int Index = InferClickedButtonIndex(CheckState);
            UpdateClickButtonState(Index);
        });

    TSharedRef<STextBlock> TextBlock = SNew(STextBlock);
    TextBlock->SetText(FText::FromString(SolutionName));
    TextBlock->SetToolTipText(FText::FromString(SolutionToolTip));

    CheckBox->SetContent(
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
            TextBlock
        ]
        + SOverlay::Slot()
        .VAlign(VAlign_Fill)
        .HAlign(HAlign_Left)
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("PlacementBrowser.ActiveTabBar"))
        ]
    );

    // update slate UI
    SolutionWidgetContainer->AddSlot().AutoHeight()[CheckBox];

    // add ref to container and mapping
    SlateWidgetRef.Push(CheckBox);
    SolutionTextMapping.Add(CheckBox, TextBlock);

    // Callback
    ensure(CB_Append);
    CB_Append(Num() - 1);
}

void FSolutionSelector::RemoveSolution(int SolutionIndex)
{
    if (SolutionIndex >= 0) {
        // remove target slot and array instance
        auto Widget = SlateWidgetRef[SolutionIndex];
        auto RemoveSlotIndex = SolutionWidgetContainer->RemoveSlot(Widget);
        ensure(RemoveSlotIndex >= 0);
        SlateWidgetRef.RemoveAt(SolutionIndex);
        SolutionTextMapping.Remove(Widget);
        // callback
        ensure(CB_Remove);
        CB_Remove(SolutionIndex);
        // update solution list UI
        UpdateClickButtonState(SolutionIndex - 1);
        UpdateToolTips();
    }
}

int FSolutionSelector::Num()
{
    ensure(SlateWidgetRef.Num() == SolutionTextMapping.Num());
    ensure(SlateWidgetRef.Num() == SolutionWidgetContainer->NumSlots());
    return SlateWidgetRef.Num();
}

void FSolutionSelector::Clear()
{
}

void FSolutionSelector::Initialize()
{
    MainLayout->AddSlot().AutoHeight()[ToolBarContainer];
    MainLayout->AddSlot().AutoHeight()[SolutionWidgetContainer];

    // Add
    TSharedRef<SWidget> BtnAdd = SNew(SButton)
        .OnClicked_Lambda([this]() -> FReply {
            AddSolution();
            return FReply::Handled();
        })
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("LevelEditor.NewLevel"))
        ];

    // Copy
    TSharedRef<SWidget> BtnCopy = SNew(SButton)
        .OnClicked_Lambda([this]() -> FReply {
            // TODO: add impl
            return FReply::Handled();
        })
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("LevelEditor.OpenLevel"))
        ];

    // Remove
    TSharedRef<SWidget> BtnRemove = SNew(SButton)
        .OnClicked_Lambda([this]() -> FReply {
            RemoveSolution(CurrentSelectedSolutionIndex);
            return FReply::Handled();
        })
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("Level.SaveDisabledIcon16x"))
        ];

    // Rename
    TSharedRef<SWidget> BtnRename = SNew(SButton)
        .OnClicked_Lambda([&]() -> FReply {
            // TODO: hint message dialog
            if (CurrentSelectedSolutionIndex < 0) {
            }
            // create input dialog
            else {
                TSharedRef<SWindow> ModalWindow = SNew(SWindow)
                    .Title(FText::FromString("Rename Solution"))
                    .HasCloseButton(true)
                    .SizingRule(ESizingRule::FixedSize)
                    .ClientSize(FVector2D(200.0f, 60.0f));
                TSharedRef<SEditableText> EditableText = SNew(SEditableText)
                    .HintText(FText::FromString("Input new solution name"));
                TSharedRef<SWidget> ResultWidget = SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    [
                        EditableText
                    ]
                    + SVerticalBox::Slot()
                    [
                        SNew(SButton)
                        .Text(FText::FromString("OK"))
                        .OnClicked_Lambda([&]() -> FReply {
                            FText Text = EditableText->GetText();
                            ModalWindow->RequestDestroyWindow();
                            RenameSolution(CurrentSelectedSolutionIndex, Text.ToString(), "TODO");
                            return FReply::Handled();
                        })
                    ];

                ModalWindow->SetContent(ResultWidget);
                GEditor->EditorAddModalWindow(ModalWindow);
            }
            return FReply::Handled();
        })
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("Level.SaveModifiedIcon16x"))
        ];

    ToolBarContainer->AddSlot().AutoWidth()[BtnAdd];
    ToolBarContainer->AddSlot().AutoWidth()[BtnCopy];
    ToolBarContainer->AddSlot().AutoWidth()[BtnRemove];
    ToolBarContainer->AddSlot().AutoWidth()[BtnRename];
}

void FSolutionSelector::UpdateClickButtonState(int CheckedIndex)
{
    // clear all box check state
    for (auto Widget : SlateWidgetRef) {
        Widget->SetIsChecked(ECheckBoxState::Unchecked);
    }
    // toggle checked box
    if (CheckedIndex >= 0) {
        ensure(CheckedIndex < Num());
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

void FSolutionSelector::UpdateToolTips()
{
    for (int i = 0; i < Num(); ++i) {
        auto Widget = SlateWidgetRef[i];
        ensure(SolutionTextMapping.Contains(Widget));
        TSharedRef<STextBlock> TextBlock = SolutionTextMapping[Widget];
        FString SolutionToolTip = FString::Printf(TEXT("The %d-th Solution"), i + 1);
        TextBlock->SetToolTipText(FText::FromString(SolutionToolTip));
    }
}

void FSolutionSelector::RenameSolution(int SolutionIndex, FString SolutionName, FString SolutionToolTip, bool Callback)
{
    ensure(SolutionIndex >= 0 && SolutionIndex < Num());
    TSharedRef<SCheckBox> Widget = SlateWidgetRef[SolutionIndex];
    ensure(SolutionTextMapping.Contains(Widget));
    TSharedRef<STextBlock> TextBlock = SolutionTextMapping[Widget];
    TextBlock->SetText(FText::FromString(SolutionName));
    TextBlock->SetToolTipText(FText::FromString(SolutionToolTip));
    if (Callback) {
        ensure(CB_Rename);
        CB_Rename(SolutionIndex, SolutionName);
    }
}

int FSolutionSelector::InferClickedButtonIndex(ECheckBoxState CheckState)
{
    if (CheckState == ECheckBoxState::Unchecked) {
        return CurrentSelectedSolutionIndex;
    }
    else if (CheckState == ECheckBoxState::Checked) {
        TArray<int> CheckedIndex;
        for (int i = 0; i < Num(); ++i) {
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
