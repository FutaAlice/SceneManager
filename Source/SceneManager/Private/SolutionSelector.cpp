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
    : SolutionTabContainer(SNew(SVerticalBox))
{    
    AppendButtons();
}

TSharedRef<SWidget> SolutionSelector::Self()
{
    return SolutionTabContainer;
}

void SolutionSelector::AddSolution(FString SolutionName, FString SolutionToolTip)
{
    int SolutionIndex = SolutionTabContainer->NumSlots() - 1;

    TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
        .Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
        .OnCheckStateChanged_Lambda([&, SolutionIndex](ECheckBoxState CheckState) {
            for (auto w : Widgets) {
                w->SetIsChecked(ECheckBoxState::Unchecked);
            }
            Widgets[SolutionIndex]->SetIsChecked(ECheckBoxState::Checked);
            ensure(CB_Active);
            CB_Active(SolutionIndex);
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
    SolutionTabContainer->InsertSlot(SolutionIndex).AutoHeight()[CheckBox];

    // add ref to container
    ensure(Widgets.Num() == SolutionIndex);
    Widgets.Push(CheckBox);

    // Callback
    ensure(CB_Append);
    CB_Append(SolutionIndex);
}

void SolutionSelector::AppendButtons()
{
    TSharedRef<SWidget> ShitButton = SNew(SButton)
        .Text(FText::FromString("ADD"))
        .OnClicked_Lambda([&]() -> FReply {
            // SolutionTabContainer.ClearChildren();
            AddSolution("FUCK", "SHIT");
            return FReply::Handled();
        });
    SolutionTabContainer->AddSlot().AutoHeight()[ShitButton];
}
