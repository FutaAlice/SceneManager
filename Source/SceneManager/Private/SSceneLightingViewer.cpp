#include "SSceneLightingViewer.h"

#include "CoreMinimal.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "SlateOptMacros.h"

#include "Engine.h" // GEngine
#include "SolutionSelector.h"

#define LOCTEXT_NAMESPACE "SceneLightViewer"

/**
 * Scene Lighting Viewer
 */
class SSceneLightingViewer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSceneLightingViewer)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

private:
    FSolutionSelector SolutionSelector;

};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSceneLightingViewer::Construct(const FArguments& InArgs)
{
    SolutionSelector.CB_Append = [](int index) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Append Index %d"), index));
    };
    SolutionSelector.CB_Remove = [](int index) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Remove Index %d"), index));
    };
    SolutionSelector.CB_Active = [](int index) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Active Index %d"), index));
    };
    SolutionSelector.CB_Rename = [](int index, FString str) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, str);
    };

    TSharedPtr<SVerticalBox> MainLayout;

    ChildSlot
    [
        SNew(SBorder)
        .Padding(4)
        .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SolutionSelector.Self()
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(MainLayout, SVerticalBox)
            ]
        ]
    ];

    MainLayout.Get()->AddSlot()
        [
            SNew(SButton)
        ];

    // MainLayout->SetContent
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace SceneLightingViewer {

FName GetTabName()
{
    const FName TabName = TEXT("SceneLightingViewer");
    return TabName;

}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnScenLightingViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(LOCTEXT("TabTitle", "Scene Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
                [
                    SNew(SSceneLightingViewer/*, &Private_MaterialSource*/)
                ]
            ];
    };

    TabManager.RegisterTabSpawner(SceneLightingViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnScenLightingViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Scene Lighting"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Lighting tab"));
}

} // namespace SceneLightingViewer

#undef LOCTEXT_NAMESPACE
