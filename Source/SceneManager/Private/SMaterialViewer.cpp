#include "SMaterialViewer.h"

#include "SlateOptMacros.h"
#include "Misc/MessageDialog.h" // FMessageDialog
#include "Widgets/SCompoundWidget.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Input/SEditableTextBox.h" // SEditableTextBox
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Layout/SBox.h"    // SBox
#include "Widgets/Layout/SScrollBox.h"  // SScrollBox
#include "Widgets/Layout/SSpacer.h" // SSpacer
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Input/SEditableText.h"    // SEditableText
#include "ContentBrowserModule.h"   // FContentBrowserModule
#include "IContentBrowserSingleton.h"   // IContentBrowserSingleton
#include "Engine/AssetManager.h"    // UAssetManager::GetAssetPathForData
#include "Editor/EditorEngine.h"    // UEditorEngine::EditorAddModalWindow
#include "Editor.h" // GEditor

#include "SolutionSelector.h"
#include "SceneManagementAssetData.h"
#include "SMaterialDetailsPanel.h"
#include "SMaterialGroupComboBox.h"

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
    void OnEditorModified();

    void CreateRenameDialog();
    void RenameGroup(FString NewName);
    void AddFromContentBrowser();

private:
    FSolutionSelector SolutionSelector;
    TSharedPtr<SHorizontalBox> Toolbar;
    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SMaterialGroupComboBox> ComboBox;

    // DEBUG
    TArray<TSharedPtr<SMaterialDetailsPanel>> MaterialDetailsPanelContainer;
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
                .AutoWidth()
                [
                    SAssignNew(ComboBox, SMaterialGroupComboBox)
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
                            AssetData->AddMaterialGroup();
                            ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Rename Selected Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        CreateRenameDialog();
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Remove Selected Group"))
                    .OnClicked_Lambda([this]() -> FReply {
                        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
                            AssetData->RemoveMaterialGroup(ComboBox->GetCurrentItemLabel().ToString());
                            ComboBox->SetCurrentItemLabel("Default");
                            ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add Empty Material"))
                    .OnClicked_Lambda([this]() -> FReply {
                        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
                            FString GroupName = ComboBox->GetCurrentItemLabel().ToString();
                            AssetData->AddMaterial(GroupName);
                            ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add From Content Browser"))
                    .OnClicked_Lambda([this]() -> FReply {
                        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
                            AddFromContentBrowser();
                            ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
                        }
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
                    SNew(SScrollBox)
                    + SScrollBox::Slot()
                    [
                        SAssignNew(MainLayout, SVerticalBox)
                    ]
                ]
            ]
        ]
    ];

    ComboBox->FUNC_GetExistGroups = [this] {
        TArray<FString> Options;
        if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
            for (auto fuck : AssetData->MaterialGroupNameList) {
                Options.Add(fuck);
            }
        }
        return Options;
    };

    ComboBox->CB_SelectionChange = [this](FString Group) {
    };

    // On solution changed
    SolutionSelector.CB_Active = [this](int SolutionIndex) {
        ForceRefresh(SolutionIndex);
        if (SolutionIndex < 0) {
            return;
        }

        //// Sync to materials
        USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
        ensure(AssetData);
        auto & AllMaterials = AssetData->MaterialSolutions[SolutionIndex]->SolutionItems;
        for (UMaterialInfo* MaterialInfo : AllMaterials) {
            MaterialInfo->ToMaterial();
        }
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
    MaterialDetailsPanelContainer.Empty();


    USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
    if (!AssetData || SolutionIndex < 0) {
        return;
    }

    for (int i = 0; i < AssetData->MaterialGroupNameList.Num(); ++i) {
        const FString GroupName = AssetData->MaterialGroupNameList[i];
        MainLayout->AddSlot()
            .AutoHeight()
            [SNew(STextBlock).Text(FText::FromString(FString("Group: ") + GroupName))];

        USolutionMaterialInfo* CurrentSolution = AssetData->MaterialSolutions[SolutionIndex];

        int BeginIndex, EndIndex;
        AssetData->GetGroupRange(GroupName, BeginIndex, EndIndex);

        for (int j = BeginIndex; j < EndIndex; ++j) {
            TSharedPtr<SMaterialDetailsPanel> DetailsPanel;
            MainLayout->AddSlot()
                .AutoHeight()
                [
                    SAssignNew(DetailsPanel, SMaterialDetailsPanel)
                ];
            UMaterialInfo *MaterialInfo = CurrentSolution->SolutionItems[j];
            DetailsPanel->BindDataField(MaterialInfo);
            MaterialDetailsPanelContainer.Add(DetailsPanel);
        }
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMaterialViewer::OnAssetDataChanged(USceneManagementAssetData* AssetData)
{
    SolutionSelector.Clear();
    if (AssetData) {
        for (int i = 0; i < AssetData->MaterialSolutionNameList.Num(); ++i) {
            const FString& SolutionName = AssetData->MaterialSolutionNameList[i];
            SolutionSelector.AddSolution(SolutionName, "", false);
        }
    }
    ForceRefresh(-1);
}

void SMaterialViewer::OnEditorModified()
{
    if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
        AssetData->SyncDataByMaterial(SolutionSelector.GetCurrentSelectedSolutionIndex());
    }
}

void SMaterialViewer::CreateRenameDialog()
{
    if (!USceneManagementAssetData::GetSelected()) {
        return;
    }

    TSharedPtr<SEditableTextBox> TextBlock;
    TSharedPtr<SWindow> ModalWindow;
    GEditor->EditorAddModalWindow(SAssignNew(ModalWindow, SWindow)
        .Title(LOCTEXT("RenameGroup", "Rename Group")) 
        .HasCloseButton(true)
        .SizingRule(ESizingRule::Autosized)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .HAlign(HAlign_Center)
            .AutoHeight()
            .Padding(5)
            [
                SAssignNew(TextBlock, SEditableTextBox)
                .HintText(FText::FromString("Input new solution name"))
                .SelectAllTextWhenFocused(true)
                .MinDesiredWidth(160)
            ]
            + SVerticalBox::Slot()
            .HAlign(HAlign_Center)
            .AutoHeight()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("OK"))
                .OnClicked_Lambda([&]() -> FReply {
                    FText Text = TextBlock->GetText();
                    ModalWindow->RequestDestroyWindow();
                    RenameGroup(Text.ToString());
                    ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
                    return FReply::Handled();
                })
            ]
        ]);
}

void SMaterialViewer::RenameGroup(FString NewName)
{
    USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
    if (!AssetData) {
        return;
    }

    FString OldName = ComboBox->GetCurrentItemLabel().ToString();
    if (AssetData->RenameMaterialGroup(OldName, NewName)) {
        ComboBox->SetCurrentItemLabel(NewName);
        ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
    }
}

void SMaterialViewer::AddFromContentBrowser()
{
    // query all selected asset
    TArray<FAssetData> AssetDataList;
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
    ContentBrowserSingleton.GetSelectedAssets(AssetDataList);

    // grep by asset type and convert to soft path
    TArray<FSoftObjectPath> Paths;
    for (auto AssetData : AssetDataList) {
        if (AssetData.GetClass()->GetFName() == "MaterialInstanceConstant") {
            Paths.Add(UAssetManager::Get().GetAssetPathForData(AssetData));
        }
    }

    // add to data field
    TSet<FSoftObjectPath> PathFailedToAdd;

    if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
        FString GroupName = ComboBox->GetCurrentItemLabel().ToString();
        int Unused;
        if (AssetData->GetGroupRange(GroupName, Unused, Unused)) {
            for (FSoftObjectPath SoftObjectPath : Paths) {
                int Err = AssetData->AddMaterial(GroupName, SoftObjectPath);
                if (Err == ERR_MAT_EXIST_INS) {
                    PathFailedToAdd.Add(SoftObjectPath);
                }
            }
        }
    }

    // hit message dialog
    if (PathFailedToAdd.Num() != 0) {
        FString args;
        for (auto SoftObjectPath : PathFailedToAdd) {
            args += "\n\t\"";
            args += SoftObjectPath.ToString();
            args += "\'";
        }
        const FText Message = FText::Format(LOCTEXT("AddFromContentBrowser_Message",
            "SoftObjectPath:{0}, already exist."), FText::AsCultureInvariant(args));
        FMessageDialog::Open(EAppMsgType::Ok, Message);
    }

    // refresh
    ForceRefresh(SolutionSelector.GetCurrentSelectedSolutionIndex());
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

void OnEditorModified()
{
    if (SMaterialViewer::MaterialViewerInstance) {
        SMaterialViewer::MaterialViewerInstance->OnEditorModified();
    }
}

} // namespace MaterialViewer

#undef LOCTEXT_NAMESPACE
