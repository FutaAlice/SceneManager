#include "SSceneLightingViewer.h"

#include "SlateOptMacros.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/SCompoundWidget.h"    // SCompoundWidget
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Layout/SSplitter.h"   // SSplitter

#include "Engine.h" // GEngine
#include "SolutionSelector.h"
#include "SLightActorComboBox.h"
#include "SLightActorDetailPanel.h"
#include "SLightActorGroup.h"
#include "InternalDataStructure.h"
#include "SceneManagementAsset.h"
#include "SSettingsView.h"
#include "EventHub.h"

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

    ~SSceneLightingViewer()
    {
        SceneLightingViewerInstance = nullptr;
    }

    void OnAssetDataChanged();

    static SSceneLightingViewer* GetInstance()
    {
        return SceneLightingViewerInstance;
    }

private:
    static SSceneLightingViewer* SceneLightingViewerInstance;

private:
    FSolutionSelector SolutionSelector;

    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SLightActorComboBox> LightActorComboBox;
    TSharedPtr<SLightActorDetailPanel> LightActorDetailPanel;
    TSharedPtr<SLightActorGroup> LightActorGroup;
};

SSceneLightingViewer* SSceneLightingViewer::SceneLightingViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSceneLightingViewer::Construct(const FArguments& InArgs)
{
    SceneLightingViewerInstance = this;

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(1, 1, 1, 1)
        .AutoWidth()
        [
            SNew(SBorder)
            .Padding(4)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                SolutionSelector.Self()
            ]
        ]
        + SHorizontalBox::Slot()
        .Padding(1, 1, 1, 1)
        [
            SNew(SBorder)
            .Padding(4)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                SAssignNew(MainLayout, SVerticalBox)
            ]
        ]
    ];

    // Key Light
    MainLayout->AddSlot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
            .Padding(2)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Key Light"))
                    .TextStyle(FEditorStyle::Get(), "LargeText")
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SAssignNew(LightActorComboBox, SLightActorComboBox)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SAssignNew(LightActorDetailPanel, SLightActorDetailPanel)
                ]
            ]
        ];

    // Aud Light
    MainLayout->AddSlot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
            .Padding(2)
            [
                SAssignNew(LightActorGroup, SLightActorGroup)
            ]
        ];

    // Set group init variables
    LightActorGroup->SolutionSelector = &SolutionSelector;
    LightActorGroup->LightCategory = LightCategory_AuxLight | LightCategory_SceneLight;


    // On data asset changed
    LightActorComboBox->CB_SelectionChange = [this](FString Name, ALight* Light) {
        LightActorDetailPanel->SetActor(Light);
    };

    // On solution changed
    SolutionSelector.CB_Active = [this](int SolutionIndex) {
        USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
        if (!SceneManagementAsset || SolutionIndex < 0) {
            LightActorDetailPanel->BindDataField(nullptr);
            return;
        }
        ULightParams* LightParams = SceneManagementAsset->GetKeyLightParamsPtr(SolutionIndex);
        // update combo box
        LightActorComboBox->SetByActorName(LightParams->ActorName);
        // update details panel
        LightActorDetailPanel->BindDataField(LightParams);
        LightActorDetailPanel->ForceRefresh();
        // update groups
        LightActorGroup->OnSolutionChanged(SolutionIndex);
    };

    // On solution rename
    SolutionSelector.CB_Rename = [](int SolutionIndex, FString SolutionName) {
        if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
            SceneManagementAsset->RenameLightingSolution(SolutionIndex, SolutionName);
        }
    };

    // On solution append
    SolutionSelector.CB_Append = [](int SolutionIndex) {
        if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
            SceneManagementAsset->AddLightingSolution();
        }
    };

    // On solution duplicate
    SolutionSelector.CB_Duplicate = [](int SolutionIndex) {
        if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
            SceneManagementAsset->DuplicateLightingSolution(SolutionIndex);
        }
    };

    // On solution remove
    SolutionSelector.CB_Remove = [](int SolutionIndex) {
        if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
            SceneManagementAsset->RemoveLightingSolution(SolutionIndex);
        }
    };
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSceneLightingViewer::OnAssetDataChanged()
{
    SolutionSelector.Clear();
    if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
        for (int i = 0; i < SceneManagementAsset->LightingSolutionNameList.Num(); ++i) {
            const FString& SolutionName = SceneManagementAsset->LightingSolutionNameList[i];
            SolutionSelector.AddSolution(SolutionName, "", false);
        }
    }
}

namespace SceneLightingViewer {

FName GetTabName()
{
    const FName TabName = TEXT("SceneLightingViewer");
    return TabName;
}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnSceneLightingViewTab = [](const FSpawnTabArgs& Args) {
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

    TabManager.RegisterTabSpawner(SceneLightingViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnSceneLightingViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Scene Lighting"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Lighting tab"));
}

void OnAssetDataChanged()
{
    SSceneLightingViewer* SceneLightingViewer = SSceneLightingViewer::GetInstance();
    SceneLightingViewer->OnAssetDataChanged();
}

} // namespace SceneLightingViewer

#undef LOCTEXT_NAMESPACE
