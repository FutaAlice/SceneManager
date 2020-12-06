#include "SSceneLightingViewer.h"

#include "CoreMinimal.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/SCompoundWidget.h"    // SCompoundWidget
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "SlateOptMacros.h"

#include "Engine.h" // GEngine
#include "SolutionSelector.h"
#include "SLightActorComboBox.h"
#include "SLightActorDetailPanel.h"

// DETAIL PANEL
#include "InternalDataStructure.h"

// CONTENT BORWSER
#include "ContentBrowserModule.h"   // FContentBrowserModule
#include "IContentBrowserSingleton.h"   // IContentBrowserSingleton
#include "AssetData.h"  // FAssetData
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

};

SSceneLightingViewer* SSceneLightingViewer::SceneLightingViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSceneLightingViewer::Construct(const FArguments& InArgs)
{
    SceneLightingViewerInstance = this;

    SolutionSelector.CB_Append = [](int SolutionIndex) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Append Index %d"), SolutionIndex));
        USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
        SceneManagementAsset->AddLightingSolution();
    };
    SolutionSelector.CB_Remove = [](int SolutionIndex) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Remove Index %d"), SolutionIndex));
        USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
        SceneManagementAsset->RemoveLightingSolution(SolutionIndex);
    };
    SolutionSelector.CB_Active = [](int SolutionIndex) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("CB_Active Index %d"), SolutionIndex));
    };
    SolutionSelector.CB_Rename = [](int SolutionIndex, FString SolutionName) {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, SolutionName);
        USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
        SceneManagementAsset->RenameLightingSolution(SolutionIndex, SolutionName);
    };

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

    MainLayout->AddSlot()
        .AutoHeight()
        [
            SNew(SButton)
            .OnClicked_Lambda([this]() -> FReply {
                FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
                IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();

                TArray<FAssetData> AssetDatas;
                ContentBrowserSingleton.GetSelectedAssets(AssetDatas);

                for (auto AssetData : AssetDatas) {

                    UE_LOG(LogTemp, Warning, TEXT("Asset %s"), *AssetData.ObjectPath.ToString());

                    FName MyAssetClassName = USceneManagementAsset::StaticClass()->GetFName();
                    UE_LOG(LogTemp, Warning, TEXT("Asset %s"), *MyAssetClassName.ToString());
                }
                return FReply::Handled();
            })
        ];

    MainLayout->AddSlot()
        .AutoHeight()
        [
            SAssignNew(LightActorComboBox, SLightActorComboBox)
        ];

    MainLayout->AddSlot()
        .AutoHeight()
        [
            SAssignNew(LightActorDetailPanel, SLightActorDetailPanel)
        ];

    LightActorComboBox->CB_SelectionChange = [this](FString Name, ALight *Light) {
        LightActorDetailPanel->SetObject(SSettingsView::GetSceneManagementAsset()->GetKeyLightParamsPtr(0));
        LightActorDetailPanel->BindActor(Light);
    };
    // MainLayout->SetContent
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSceneLightingViewer::OnAssetDataChanged()
{
    USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
    SolutionSelector.Clear();

    for (int i = 0; i < SceneManagementAsset->LightingSolutionNameList.Num(); ++i) {
        const FString& SolutionName = SceneManagementAsset->LightingSolutionNameList[i];
        SolutionSelector.AddSolution(SolutionName, "", false);
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
