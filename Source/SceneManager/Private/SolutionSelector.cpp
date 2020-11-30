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

#define LOCTEXT_NAMESPACE "SolutionSelectorCommands"

/**
 * @brief A set of actions of Scene Manager main frame
 *
 */
struct FSolutionSelectorCommands : public TCommands<FSolutionSelectorCommands>
{
    FSolutionSelectorCommands()
        : TCommands<FSolutionSelectorCommands>(
            TEXT("SolutionSelector"), // Context name for fast lookup
            LOCTEXT("SolutionSelector FUCK", "SolutionSelector SHIT"), // Localized context name for displaying
            NAME_None, // Parent
            FCoreStyle::Get().GetStyleSetName() // Icon Style Set
            )
    {
    }

    // TCommand<> interface
    virtual void RegisterCommands() override;
    // End of TCommand<> interface

    TSharedPtr<FUICommandInfo> Action_Rename;
    TSharedPtr<FUICommandInfo> Action_Remove;
};

void FSolutionSelectorCommands::RegisterCommands()
{
    UI_COMMAND(Action_Rename, "Action_Rename", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(Action_Remove, "Action_Remove", "TODO", EUserInterfaceActionType::Check, FInputChord());
}

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

    // build command list for tab restoration menu:
    TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());
    TWeakPtr<SCheckBox> OwningWidgetWeak = CheckBox;


    CheckBox->SetOnMouseButtonUp(
        FPointerEventHandler::CreateStatic(
            [](
                const FGeometry&,   // The geometry of the widget
                const FPointerEvent& PointerEvent,  // The Mouse Event that we are processing
                TWeakPtr<SCheckBox> InOwnerWeak,
                TSharedPtr<FUICommandList> InCommandList) -> FReply {

                    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Append Index %d"), 1212));

                    if (PointerEvent.GetEffectingButton() == EKeys::RightMouseButton) {
                        // if the tab manager is still available then make a context window that allows users to
                        // show and hide tabs:
                        TSharedPtr<SCheckBox> InOwner = InOwnerWeak.Pin();
                        if (InOwner.IsValid()) {
                            FMenuBuilder MenuBuilder(true, InCommandList);

                            MenuBuilder.PushCommandList(InCommandList.ToSharedRef());
                            MenuBuilder.AddMenuEntry(FSolutionSelectorCommands::Get().Action_Rename);
                            MenuBuilder.AddMenuEntry(FSolutionSelectorCommands::Get().Action_Remove);
                            MenuBuilder.PopCommandList();

                            FWidgetPath WidgetPath = PointerEvent.GetEventPath() != nullptr ? *PointerEvent.GetEventPath() : FWidgetPath();
                            FSlateApplication::Get().PushMenu(InOwner.ToSharedRef(), WidgetPath, MenuBuilder.MakeWidget(), PointerEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

                            return FReply::Handled();
                        }
                    }
                    return FReply::Unhandled();
            }
            , OwningWidgetWeak , CommandList));

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

#undef LOCTEXT_NAMESPACE
