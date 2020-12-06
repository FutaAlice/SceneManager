#include "SCharacterLightingViewer.h"
#include "SlateOptMacros.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "EditorStyleSet.h" // FEditorStyle
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Text/STextBlock.h"    // STextBlock

#include "Engine.h" // GEngine

#include "SSettingsView.h"
#include "SolutionSelector.h"
#include "SceneManagementAsset.h"

#define LOCTEXT_NAMESPACE "CharacterLightViewer"

/**
 *
 */
class SCharacterLightingViewer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCharacterLightingViewer)
	{
	}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

    ~SCharacterLightingViewer()
    {
        CharacterLightingViewerInstance = nullptr;
    }

    void OnAssetDataChanged();

    static SCharacterLightingViewer* GetInstance()
    {
        return CharacterLightingViewerInstance;
    }

private:
    static SCharacterLightingViewer* CharacterLightingViewerInstance;
    FSolutionSelector SolutionSelector;
    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SVerticalBox> LightContainer;
    TSharedPtr<SHorizontalBox> ToolBarContainer;
};

SCharacterLightingViewer* SCharacterLightingViewer::CharacterLightingViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCharacterLightingViewer::Construct(const FArguments& InArgs)
{
    CharacterLightingViewerInstance = this;

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

    MainLayout->AddSlot().AutoHeight()[SAssignNew(ToolBarContainer, SHorizontalBox)];
    MainLayout->AddSlot().AutoHeight()[SAssignNew(LightContainer, SVerticalBox)];

    // Create Toolbar

    TSharedRef<SWidget> TextBlock = SNew(STextBlock)
        .Text(FText::FromString("Aux Light Params: "));

    TSharedRef<SWidget> BtnAdd = SNew(SButton)
        .Text(FText::FromString("Add"))
        .OnClicked_Lambda([this]() -> FReply {
            return FReply::Handled();
        });
    
    TSharedRef<SWidget> BtnRemove = SNew(SButton)
        .Text(FText::FromString("Remove"))
        .OnClicked_Lambda([this]() -> FReply {
            return FReply::Handled();
        });

    ToolBarContainer->AddSlot().VAlign(VAlign_Center).AutoWidth()[TextBlock];
    ToolBarContainer->AddSlot().AutoWidth()[BtnAdd];
    ToolBarContainer->AddSlot().AutoWidth()[BtnRemove];

}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCharacterLightingViewer::OnAssetDataChanged()
{
    USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset();
    SolutionSelector.Clear();

    for (int i = 0; i < SceneManagementAsset->LightingSolutionNameList.Num(); ++i) {
        const FString& SolutionName = SceneManagementAsset->LightingSolutionNameList[i];
        SolutionSelector.AddSolution(SolutionName, "", false);
    }
}

namespace CharacterLightingViewer {

FName GetTabName()
{
    const FName TabName = TEXT("CharacterLightingViewer");
    return TabName;
}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnCharacterLightingViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(LOCTEXT("TabTitle", "Character Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
            [
                SNew(SCharacterLightingViewer/*, &Private_MaterialSource*/)
            ]
            ];
    };

    TabManager.RegisterTabSpawner(CharacterLightingViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnCharacterLightingViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Scene Lighting"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Lighting tab"));
}

void OnAssetDataChanged()
{
    SCharacterLightingViewer* CharacterLightingViewer = SCharacterLightingViewer::GetInstance();
    CharacterLightingViewer->OnAssetDataChanged();
}

} // namespace SceneLightingViewer

#undef LOCTEXT_NAMESPACE
