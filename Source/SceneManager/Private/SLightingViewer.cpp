#include "SLightingViewer.h"

#include "SlateOptMacros.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/SCompoundWidget.h"    // SCompoundWidget
#include "Widgets/Docking/SDockTab.h"   // SDockTab

#include "SolutionSelector.h"
#include "SLightActorComboBox.h"
#include "SLightActorDetailPanel.h"
#include "SLightActorGroup.h"
#include "SMPCDetialsPanel.h"
#include "SceneManagementAssetData.h"
#include "EventHub.h"

/**
 * Scene Lighting Viewer
 */
class SLightingViewer : public SCompoundWidget
{
    // Constructor and Destructor
public:
    SLATE_BEGIN_ARGS(SLightingViewer)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs, ELightCategory LightCategory);

    ~SLightingViewer();

    // Callbacks
public:
    void OnAssetDataChanged(USceneManagementAssetData* AssetData);
    void OnMPCChanged(UMaterialParameterCollection* MPC);
    void OnOtherInstanceModified(SLightingViewer* Modifier, int RemovedIndex);

    // Static Fucntions
public:
    static SLightingViewer*
        GetInstance(ELightCategory LightCategory);
    static void
        SetInstance(ELightCategory LightCategory, SLightingViewer* Instance);
    static void
        NotifyAssetDataModified(SLightingViewer* Modifier, int RemovedIndex = -1);
    
    // Static Members
private:
    static SLightingViewer* SceneLightingViewerInstance;
    static SLightingViewer* CharacterLightingViewerInstance;

    // Instance Members
private:
    FSolutionSelector SolutionSelector;

    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SLightActorComboBox> LightActorComboBox;
    TSharedPtr<SLightActorDetailPanel> LightActorDetailPanel;
    TSharedPtr<SLightActorGroup> LightActorGroup;
    TSharedPtr<SMPCDetialsPanel> MPCDetailsPanel;
};

SLightingViewer* SLightingViewer::SceneLightingViewerInstance = nullptr;
SLightingViewer* SLightingViewer::CharacterLightingViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightingViewer::Construct(const FArguments& InArgs, ELightCategory LightCategory)
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
    SetInstance(LightCategory, this);
    
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
        USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
        if (!AssetData) {
            LightActorDetailPanel->BindDataField(nullptr);
            return;
        }
        ULightParams* LightParams = AssetData->GetKeyLightParamsPtr(SolutionIndex);
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
    SolutionSelector.CB_Rename = [this](int SolutionIndex, FString SolutionName) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->RenameLightingSolution(SolutionIndex, SolutionName);
            NotifyAssetDataModified(this);
        }
    };

    // On solution append
    SolutionSelector.CB_Append = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->AddLightingSolution();
            NotifyAssetDataModified(this);
        }
    };

    // On solution duplicate
    SolutionSelector.CB_Duplicate = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->DuplicateLightingSolution(SolutionIndex);
            NotifyAssetDataModified(this);
        }
    };

    // On solution remove
    SolutionSelector.CB_Remove = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->RemoveLightingSolution(SolutionIndex);
            NotifyAssetDataModified(this, SolutionIndex);
        }
    };
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SLightingViewer::~SLightingViewer()
{
    if (SceneLightingViewerInstance == this) {
        SceneLightingViewerInstance = nullptr;
    }
    if (CharacterLightingViewerInstance == this) {
        CharacterLightingViewerInstance = nullptr;
    }
}

void SLightingViewer::OnAssetDataChanged(USceneManagementAssetData * AssetData)
{
    SolutionSelector.Clear();
    LightActorGroup->Clear();
    LightActorDetailPanel->BindDataField(nullptr);
    if (AssetData) {
        for (int i = 0; i < AssetData->LightingSolutionNameList.Num(); ++i) {
            const FString& SolutionName = AssetData->LightingSolutionNameList[i];
            SolutionSelector.AddSolution(SolutionName, "", false);
        }
    }
}

void SLightingViewer::OnMPCChanged(UMaterialParameterCollection* MPC)
{
    MPCDetailsPanel->SetObject(MPC);
}

void SLightingViewer::OnOtherInstanceModified(SLightingViewer * Modifier, int RemovedIndex)
{
    if (Modifier == this) {
        return;
    }
    int PrevIndex = SolutionSelector.GetCurrentSelectedSolutionIndex();
    if (RemovedIndex >= 0 && RemovedIndex <= PrevIndex) {
        PrevIndex -= 1;
    }
    OnAssetDataChanged(USceneManagementAssetData::GetSelected());
    SolutionSelector.UpdateClickButtonState(PrevIndex);
}

SLightingViewer * SLightingViewer::GetInstance(ELightCategory LightCategory)
{
    if (LightCategory & LightCategory_SceneLight)
        return SceneLightingViewerInstance;
    else if (LightCategory & LightCategory_CharacterLight)
        return CharacterLightingViewerInstance;
    else
        return nullptr;
}

void SLightingViewer::SetInstance(ELightCategory LightCategory, SLightingViewer * Instance)
{
    if (LightCategory & LightCategory_SceneLight)
        SceneLightingViewerInstance = Instance;
    else if (LightCategory & LightCategory_CharacterLight)
        CharacterLightingViewerInstance = Instance;
    else
        throw;
}

void SLightingViewer::NotifyAssetDataModified(SLightingViewer * Modifier, int RemovedIndex)
{
    ensure(SceneLightingViewerInstance);
    ensure(CharacterLightingViewerInstance);
    SceneLightingViewerInstance->OnOtherInstanceModified(Modifier, RemovedIndex);
    CharacterLightingViewerInstance->OnOtherInstanceModified(Modifier, RemovedIndex);
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

void RegisterTabSpawner(FTabManager& TabManager, ELightCategory LightCategory)
{
    bool bs = LightCategory & LightCategory_SceneLight;
    bool bc = LightCategory & LightCategory_CharacterLight;

    ensure(bs || bc); // is scene or character lighting
    ensure(!(bs && bc)); // not both scene and character lighting

    FString CategoryString = bs ? "Scene" : "Character";

    const auto SpawnSceneLightingViewTab = [LightCategory, CategoryString](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(FText::FromString(CategoryString + " Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
                [
                    SNew(SLightingViewer, LightCategory)
                ]
            ];
    };

    TabManager.RegisterTabSpawner(LightingViewer::GetTabName(LightCategory), FOnSpawnTab::CreateLambda(SpawnSceneLightingViewTab))
        .SetDisplayName(FText::FromString(CategoryString + " Lighting"));
}

void OnAssetDataChanged(USceneManagementAssetData * AssetData)
{
    SLightingViewer* SceneLightingViewer = SLightingViewer::GetInstance(LightCategory_SceneLight);
    SceneLightingViewer->OnAssetDataChanged(AssetData);
    SLightingViewer* CharacterLightingViewer = SLightingViewer::GetInstance(LightCategory_CharacterLight);
    CharacterLightingViewer->OnAssetDataChanged(AssetData);
}

void OnMPCChanged(UMaterialParameterCollection* MPC, int LightCategory)
{
    SLightingViewer* LightingViewer = SLightingViewer::GetInstance(LightCategory);
    LightingViewer->OnMPCChanged(MPC);
}

} // namespace LightingViewer
