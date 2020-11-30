// Fill out your copyright notice in the Description page of Project Settings.
#include "SMaterialViewer.h"

#include "CoreMinimal.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Layout/SSpacer.h" // SSpacer
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "CallStackViewer"

/**
 *
 */
class SMaterialViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMaterialViewer)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);
};


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialViewer::Construct(const FArguments& InArgs)
{
    TSharedRef<SVerticalBox> Tabs = SNew(SVerticalBox);
    Tabs->AddSlot()
        .AutoHeight()
        [

            SNew(SCheckBox)
            .Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
            // .OnCheckStateChanged(this, &SSceneManagerTools::OnPlacementTabChanged, Info.UniqueHandle)
            // .IsChecked(this, &SSceneManagerTools::GetPlacementTabCheckedState, Info.UniqueHandle)
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
                    .Text(LOCTEXT("FUCK", "FUCK"))
                    .ToolTipText(LOCTEXT("FUCK1", "FUCK1"))
                ]

                + SOverlay::Slot()
                .VAlign(VAlign_Fill)
                .HAlign(HAlign_Left)
                [
                    SNew(SImage)
                    .Image(FEditorStyle::GetBrush("PlacementBrowser.ActiveTabBar"))
                ]
            ]
        ];


    ChildSlot
    [
        // Populate the widget
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            Tabs
        ]
        
    ];
    
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


namespace MaterialViewer {

FName GetTabName()
{
    const FName TabName = TEXT("MaterialViewer");
    return TabName;

}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnMaterialViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(LOCTEXT("TabTitle", "Material"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
                [
                    SNew(SMaterialViewer/*, &Private_MaterialSource*/)
                ]
            ];
    };

    TabManager.RegisterTabSpawner(MaterialViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnMaterialViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Material"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Material tab"));

}

} // namespace MaterialViewer

#undef LOCTEXT_NAMESPACE
