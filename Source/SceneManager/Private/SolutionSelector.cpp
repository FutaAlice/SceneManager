#include "SolutionSelector.h"

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
    TSharedRef<SWidget> CheckBox = SNew(SCheckBox)
        .Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
        .OnCheckStateChanged_Lambda([](ECheckBoxState CheckState) {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
                FString::Printf(TEXT("World delta for current frame equals %d"), (int)CheckState));
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

    SolutionTabContainer->AddSlot().AutoHeight()[CheckBox];
}

void SolutionSelector::AppendButtons()
{
    TSharedRef<SWidget> ShitButton = SNew(SButton)
        .Text(FText::FromString("ADD"))
        .OnClicked_Lambda([&]() -> FReply {
            AddSolution("FUCK", "SHIT");
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
                FString::Printf(TEXT("World delta for current frame equals %f"), 0.2f));
            return FReply::Handled();
        });
    SolutionTabContainer->AddSlot().AutoHeight()[ShitButton];
}
