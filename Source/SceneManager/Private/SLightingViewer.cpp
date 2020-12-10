#include "SLightingViewer.h"

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
#include "SMPCDetialsPanel.h"
#include "SceneManagementAsset.h"
#include "SSettingsView.h"
#include "EventHub.h"

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
    void Construct(const FArguments& InArgs, ELightCategory LightCategory);

    ~SSceneLightingViewer()
    {
        SceneLightingViewerInstance = nullptr;
        CharacterLightingViewerInstance = nullptr;
    }

    void OnAssetDataChanged();
    void OnMPCChanged();

    void DebugSyncLightingSolutionRename(int SolutionIndex, FString SolutionName)
    {
        SolutionSelector.Clear();
        LightActorGroup->Clear();
        LightActorDetailPanel->BindDataField(nullptr);
        OnAssetDataChanged();
    }

    static SSceneLightingViewer* GetInstance(ELightCategory LightCategory)
    {
        if (LightCategory | LightCategory_SceneLight)
            return SceneLightingViewerInstance;
        else if (LightCategory | LightCategory_CharacterLight)
            return CharacterLightingViewerInstance;
        else
            return nullptr;
    }

    static void SetInstance(SSceneLightingViewer* Instance, ELightCategory LightCategory)
    {
        if (LightCategory | LightCategory_SceneLight)
            SceneLightingViewerInstance = Instance;
        else if (LightCategory | LightCategory_CharacterLight)
            CharacterLightingViewerInstance = Instance;
        else
            throw;
    }
    
private:
    static SSceneLightingViewer* SceneLightingViewerInstance;
    static SSceneLightingViewer* CharacterLightingViewerInstance;

private:
    FSolutionSelector SolutionSelector;

    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SLightActorComboBox> LightActorComboBox;
    TSharedPtr<SLightActorDetailPanel> LightActorDetailPanel;
    TSharedPtr<SLightActorGroup> LightActorGroup;
    TSharedPtr<SMPCDetialsPanel> MPCDetailsPanel;
};

SSceneLightingViewer* SSceneLightingViewer::SceneLightingViewerInstance = nullptr;
SSceneLightingViewer* SSceneLightingViewer::CharacterLightingViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSceneLightingViewer::Construct(const FArguments& InArgs, ELightCategory LightCategory)
{
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

    // Aud MPC
    MainLayout->AddSlot()
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
            .Padding(2)
            [
                SAssignNew(MPCDetailsPanel, SMPCDetialsPanel)
            ]
        ];

    // Set selector pointer
    LightActorGroup->SolutionSelector = &SolutionSelector;

    // Set group init variables
    ensure(LightCategory == LightCategory_SceneLight || LightCategory == LightCategory_CharacterLight);
    LightActorGroup->LightCategory = LightCategory | LightCategory_AuxLight;
    SetInstance(this, LightCategory);
    
    // On data asset changed
    LightActorComboBox->CB_SelectionChange = [this](FString Name, ALight* Light) {
        LightActorDetailPanel->SetActor(Light);
    };

    // On solution changed
    SolutionSelector.CB_Active = [this](int SolutionIndex) {
        if (SolutionIndex < 0) {
            LightActorDetailPanel->BindDataField(nullptr);
            return;
        }
        USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
        if (!SceneManagementAsset) {
            LightActorDetailPanel->BindDataField(nullptr);
            return;
        }
        ULightParams* LightParams = SceneManagementAsset->GetKeyLightParamsPtr(SolutionIndex);
        // update combo box
        LightActorComboBox->SetByActorName(LightParams->ActorName);
        // update details panel
        LightActorDetailPanel->BindDataField(LightParams);
        LightActorDetailPanel->ForceRefresh();
        // Sync to actor
        LightParams->ToActor();
        // update groups
        LightActorGroup->OnSolutionChanged(SolutionIndex);
    };

    // On solution rename
    SolutionSelector.CB_Rename = [](int SolutionIndex, FString SolutionName) {
        if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
            SceneManagementAsset->RenameLightingSolution(SolutionIndex, SolutionName);
            // CharacterLightingViewer::DebugSyncLightingSolutionRename(SolutionIndex, SolutionName);
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
            // CharacterLightingViewer::DebugSyncLightingSolutionRename(0, "");
        }
    };
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSceneLightingViewer::OnAssetDataChanged()
{
    SolutionSelector.Clear();
    LightActorGroup->OnSolutionChanged(-1);
    if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset(false)) {
        for (int i = 0; i < SceneManagementAsset->LightingSolutionNameList.Num(); ++i) {
            const FString& SolutionName = SceneManagementAsset->LightingSolutionNameList[i];
            SolutionSelector.AddSolution(SolutionName, "", false);
        }
    }
}

void SSceneLightingViewer::OnMPCChanged()
{
    if (UMaterialParameterCollection* MPC = SSettingsView::GetSceneLightingMPC()) {
        MPCDetailsPanel->SetObject(MPC);
    }
}

namespace LightingViewer {

FName GetTabName(ELightCategory LightCategory)
{
    if (LightCategory & LightCategory_SceneLight) {
        const FName TabName = TEXT("SceneLightingViewer");
        return TabName;
    }
    else if (LightCategory & LightCategory_CharacterLight) {
        const FName TabName = TEXT("CharacterLightingViewer");
        return TabName;
    }
    else {
        ensure(false);
        return FName();
    }
}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnSceneLightingViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(FText::FromString("Scene Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
            [
                SNew(SSceneLightingViewer, LightCategory_SceneLight)
            ]
        ];
    };

    TabManager.RegisterTabSpawner(LightingViewer::GetTabName(LightCategory_SceneLight), FOnSpawnTab::CreateStatic(SpawnSceneLightingViewTab))
        .SetDisplayName(FText::FromString("Scene Lighting"))
        .SetTooltipText(FText::FromString("Open the Scene Lighting tab"));

    const auto SpawnCharacterLightingViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(FText::FromString("Character Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
            [
                SNew(SSceneLightingViewer, LightCategory_CharacterLight)
            ]
            ];
    };

    TabManager.RegisterTabSpawner(LightingViewer::GetTabName(LightCategory_CharacterLight), FOnSpawnTab::CreateStatic(SpawnCharacterLightingViewTab))
        .SetDisplayName(FText::FromString("Character Lighting"))
        .SetTooltipText(FText::FromString("Open the Character Lighting tab"));
}

void OnAssetDataChanged()
{
    SSceneLightingViewer* SceneLightingViewer = SSceneLightingViewer::GetInstance(LightCategory_SceneLight);
    SceneLightingViewer->OnAssetDataChanged();
    SSceneLightingViewer* CharacterLightingViewer = SSceneLightingViewer::GetInstance(LightCategory_CharacterLight);
    CharacterLightingViewer->OnAssetDataChanged();
}

void OnMPCChanged()
{
    SSceneLightingViewer* SceneLightingViewer = SSceneLightingViewer::GetInstance(LightCategory_SceneLight);
    SceneLightingViewer->OnMPCChanged();
    SSceneLightingViewer* CharacterLightingViewer = SSceneLightingViewer::GetInstance(LightCategory_CharacterLight);
    CharacterLightingViewer->OnMPCChanged();
}

void DebugSyncLightingSolutionRename(int SolutionIndex, FString SolutionName)
{
    //if (SCharacterLightingViewer* CharacterLightingViewer = SCharacterLightingViewer::GetInstance()) {
    //    CharacterLightingViewer->DebugSyncLightingSolutionRename(SolutionIndex, SolutionName);
    //}
}

} // namespace LightingViewer

