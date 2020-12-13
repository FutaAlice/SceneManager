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

#define LOCTEXT_NAMESPACE "MaterialViewer"

TSharedRef<SBoxPanel> CreateVectorParamSlot(FString name, FLinearColor value)
{
    return
        SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromString(name))
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(5)
            ]
            + SHorizontalBox::Slot()
            .Padding(10, 0, 10, 0)
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor(value))
                // .OnMouseButtonDown(this, &SMaterialGroupItemWidget::OnClickColorBlock, name, value, index)
            ];
}

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

    static SMaterialViewer* MaterialViewerInstance;
    void OnAssetDataChanged(USceneManagementAssetData* AssetData);

private:
    FSolutionSelector SolutionSelector;
    TSharedPtr<SHorizontalBox> Toolbar;
    TSharedPtr<SVerticalBox> MainLayout;

    // DEBUG
    IDetailsView* DetailsView;
    UMaterialInfo* MaterialInfo;
    TSharedPtr<SUniformGridPanel> UniformGridPanel;

    UMaterialInstance* MaterialInstance;

    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
    {
        FSoftObjectPath SoftObjectPath = MaterialInfo->SoftObjectPath;
        if (SoftObjectPath.IsNull()) {
            UniformGridPanel->ClearChildren();
            return;
        }
        UObject* Instance = SoftObjectPath.ResolveObject();
        if (!Instance) {
            Instance = SoftObjectPath.TryLoad();
        }

        MaterialInstance = Cast<UMaterialInstance>(Instance);
        ensure(MaterialInstance);


        if (MaterialInstance) {
            TArray<FMaterialParameterInfo> ParameterInfo;
            TArray<FGuid> ParameterGuids;

            MaterialInstance->GetAllVectorParameterInfo(ParameterInfo, ParameterGuids);
            FString Name = ParameterInfo[0].Name.ToString();
            FLinearColor VectorValue;
            MaterialInstance->GetVectorParameterValue(ParameterInfo[0], VectorValue);

            UniformGridPanel->AddSlot(0, 0)
                [
                    CreateVectorParamSlot(Name, FLinearColor(1, 0, 0, 0))
                ];
            UniformGridPanel->AddSlot(1, 0)
                [
                    CreateVectorParamSlot(Name, FLinearColor(0, 1, 0, 0.5))
                ];
            UniformGridPanel->AddSlot(0, 1)
                [
                    CreateVectorParamSlot(Name, FLinearColor(0, 0, 1, 1))
                ];
            //MaterialInstance->GetVectorParameterValue(ParameterInfo[0], Color);
            //DetailsView->SetObject(&Color);
        }
    }
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
        + SHorizontalBox::Slot()
        .Padding(1, 1, 1, 1)
        [
            SAssignNew(UniformGridPanel, SUniformGridPanel)
        ]
    ];

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMaterialViewer::OnFinishedChangingProperties);
    DetailsView = DetailsViewRef.operator->();

    MainLayout->AddSlot().AutoHeight()[DetailsViewRef];



}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMaterialViewer::OnAssetDataChanged(USceneManagementAssetData* AssetData)
{
    if (AssetData) {
        if (!AssetData->TestMaterialInfo) {
            AssetData->TestMaterialInfo = NewObject<UMaterialInfo>(AssetData);
        }
        MaterialInfo = AssetData->TestMaterialInfo;
        DetailsView->SetObject(AssetData->TestMaterialInfo);
    }
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
