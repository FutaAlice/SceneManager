#include "SPostProcessViewer.h"

#include "SlateOptMacros.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "Widgets/SCompoundWidget.h"    // SCompoundWidget
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "Widgets/Layout/SScrollBox.h"  // SScrollBox
#include "Engine/PostProcessVolume.h"   // APostProcessVolume

#include "SPostProcessComboBox.h"
//#include "SLightActorDetailPanel.h"
//#include "SLightActorGroup.h"
//#include "SMPCDetialsPanel.h"
//#include "SceneManagementAssetData.h"
//#include "EventHub.h"

#define LOCTEXT_NAMESPACE "PostProcessViewer"

/**
 * Scene Lighting Viewer
 */
class SPostProcessViewer : public SCompoundWidget
{
    // Constructor and Destructor
public:
    SLATE_BEGIN_ARGS(SPostProcessViewer)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    // Instance Members
private:
    TSharedPtr<SPostProcessComboBox> PostProcessComboBox;
    IDetailsView *DetailsView;
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPostProcessViewer::Construct(const FArguments& InArgs)
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsView = DetailsViewRef.operator->();
    //DetailsView->OnFinishedChangingProperties().AddRaw(this, &SMPCDetialsPanel::OnFinishedChangingProperties);
    //DetailsView->RegisterInstancedCustomPropertyLayout(UMaterialParameterCollection::StaticClass(),
    //    FOnGetDetailCustomizationInstance::CreateRaw(this, &SMPCDetialsPanel::CreateDetailCustomizationInstance));

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
                SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SAssignNew(PostProcessComboBox, SPostProcessComboBox)
                ]
                + SScrollBox::Slot()
                [
                    DetailsViewRef
                ]
            ]
        ]
    ];

    PostProcessComboBox->CB_SelectionChange = [this](FString, APostProcessVolume *Volume) {
        DetailsView->SetObject(Volume);
    };
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace PostProcessViewer {

FName GetTabName()
{
    const FName TabName = TEXT("PostProcessView");
    return TabName;
}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnMaterialViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(LOCTEXT("TabTitle", "PostProcess"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
                [
                    SNew(SPostProcessViewer/*, &Private_MaterialSource*/)
                ]
            ];
    };

    TabManager.RegisterTabSpawner(PostProcessViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnMaterialViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "PostProcess"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the PostProcess tab"));
}

} // namespace PostProcessViewer

#undef LOCTEXT_NAMESPACE
