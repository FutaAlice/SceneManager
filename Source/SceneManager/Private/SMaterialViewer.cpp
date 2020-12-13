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

#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule

#include "SolutionSelector.h"
#include "SceneManagementAssetData.h"

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

    // Static Members
private:
    static SMaterialViewer* MaterialViewerInstance;

    // Instance Members
private:
    FSolutionSelector SolutionSelector;
    TSharedPtr<SVerticalBox> MainLayout;

    // DEBUG
    IDetailsView* DetailsView;
    UMaterialInfo* MaterialInfo;
    UMaterialInstance* MaterialInstance;
    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
    {
        FSoftObjectPath SoftObjectPath = MaterialInfo->SoftObjectPath;
        UObject* Instance = SoftObjectPath.ResolveObject();
        if (!Instance) {
            Instance = SoftObjectPath.TryLoad();
        }
        ensure(Instance);

        MaterialInstance = Cast<UMaterialInstance>(Instance);
    }
};


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialViewer::Construct(const FArguments& InArgs)
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

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    MainLayout->AddSlot().AutoHeight()[DetailsViewRef];

    MaterialInfo = NewObject<UMaterialInfo>();
    MaterialInfo->AddToRoot();

    DetailsViewRef->SetObject(MaterialInfo);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMaterialViewer::OnFinishedChangingProperties);
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
