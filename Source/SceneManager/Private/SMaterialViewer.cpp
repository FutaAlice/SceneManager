#include "SMaterialViewer.h"

#include "SlateOptMacros.h"
#include "Widgets/SCompoundWidget.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Layout/SSpacer.h" // SSpacer
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Layout/SBox.h"    // SBox

#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Widgets/Layout/SUniformGridPanel.h"   // SUniformGridPanel

#include "SolutionSelector.h"
#include "SceneManagementAssetData.h"
#include "SMaterialDetailsPanel.h"

#define LOCTEXT_NAMESPACE "MaterialViewer"

/**
 * Material Viewer
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
    void ForceRefresh(int SolutionIndex);

    static SMaterialViewer* MaterialViewerInstance;
    void OnAssetDataChanged(USceneManagementAssetData* AssetData);

private:
    FSolutionSelector SolutionSelector;
    TSharedPtr<SHorizontalBox> Toolbar;
    TSharedPtr<SVerticalBox> MainLayout;

    // DEBUG
    // TSharedPtr<SMaterialDetailsPanel> MaterialDetailsPanel;
};

SMaterialViewer* SMaterialViewer::MaterialViewerInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialViewer::Construct(const FArguments& InArgs)
{
    MaterialViewerInstance = this;

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
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .Padding(1, 1, 1, 1)
            .AutoHeight()
            [
                // Toolbar
                SAssignNew(Toolbar, SHorizontalBox)
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Rename Selected Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Remove Selected Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add From Content Browser"))
                    .OnClicked_Lambda([this]() -> FReply {
                        return FReply::Handled();
                    })
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(1, 1, 1, 1)
            [
                SNew(SBorder)
                .Padding(4)
                .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                [
                    SAssignNew(MainLayout, SVerticalBox)
                ]
            ]
        ]
        //+ SHorizontalBox::Slot()
        //.Padding(1, 1, 1, 1)
        //[
        //    SAssignNew(MaterialDetailsPanel, SMaterialDetailsPanel)
        //]
    ];

    // On solution changed
    SolutionSelector.CB_Active = [this](int SolutionIndex) {
        ForceRefresh(SolutionIndex);
        //// Sync to actor
        //LightParams->ToActor();
        //// update groups
        //LightActorGroup->OnSolutionChanged(SolutionIndex);
    };
    
    // On solution rename
    SolutionSelector.CB_Rename = [this](int SolutionIndex, FString SolutionName) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->RenameMaterialSolution(SolutionIndex, SolutionName);
        }
    };

    // On solution append
    SolutionSelector.CB_Append = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->AddMaterialSolution();
        }
    };

    // On solution duplicate
    SolutionSelector.CB_Duplicate = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            // AssetData->DuplicateMaterialSolution(SolutionIndex);
        }
    };

    // On solution remove
    SolutionSelector.CB_Remove = [this](int SolutionIndex) {
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            AssetData->RemoveMaterialSolution(SolutionIndex);
        }
    };
}

void SMaterialViewer::ForceRefresh(int SolutionIndex)
{
    MainLayout->ClearChildren();
    USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
    if (!AssetData) {
        // TODO
        return;
    }

    for (int i = 0; i < AssetData->MaterialGroupNameList.Num(); ++i) {
        const FString GroupName = AssetData->MaterialGroupNameList[i];
        MainLayout->AddSlot()[SNew(STextBlock).Text(FText::FromString(GroupName + " Group"))];

        USolutionMaterialInfo* CurrentSolution = AssetData->MaterialSolutions[SolutionIndex];
        const int GroupBeginIndex = AssetData->MaterialGroupIndexList[i];
        const int GroupEndIndex = (i == AssetData->MaterialGroupNameList.Num() - 1) ?   // is the last group ?
            CurrentSolution->SolutionItems.Num() :
            AssetData->MaterialGroupIndexList[i + 1];

        for (int j = GroupBeginIndex; j < GroupEndIndex; ++j) {
            // TODO
            CurrentSolution[j];
        }
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMaterialViewer::OnAssetDataChanged(USceneManagementAssetData* AssetData)
{
    SolutionSelector.Clear();
    if (AssetData) {
        //if (!AssetData->TestMaterialInfo) {
        //    AssetData->TestMaterialInfo = NewObject<UMaterialInfo>(AssetData);
        //}
        // MaterialDetailsPanel->BindDataField(AssetData->TestMaterialInfo);
    }
    ForceRefresh(-1);
}

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

void OnAssetDataChanged(USceneManagementAssetData* AssetData)
{
    if (SMaterialViewer::MaterialViewerInstance) {
        SMaterialViewer::MaterialViewerInstance->OnAssetDataChanged(AssetData);
    }
}

} // namespace MaterialViewer

#undef LOCTEXT_NAMESPACE
