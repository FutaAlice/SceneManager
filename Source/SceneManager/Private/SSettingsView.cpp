#include "SSettingsView.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule

#include "Widgets/SBoxPanel.h"  // SVerticalBox, SHorizontalBox
#include "Widgets/Input/SButton.h"  // SButton
#include "InternalDataStructure.h"
#include "SceneManagementAsset.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsView::Construct(const FArguments& InArgs)
{
    SelectedUXXX = NewObject<UXXX>();
    SelectedUXXX->AddToRoot();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightView->SetObject(SelectedUXXX);
    PlayerLightView->OnFinishedChangingProperties().AddRaw(this, &SSettingsView::OnFinishedChangingProperties);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SButton)//PlayerLightView
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            PlayerLightView
        ]
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSettingsView::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    USceneManagementAsset *MyAsset = SelectedUXXX->MyAsset;
    if (!MyAsset) {
        return;
    }
    for (auto Name : MyAsset->LightingSolutionNameList) {
        UE_LOG(LogTemp, Warning, TEXT("MyAsset: %s"), *Name);
    }
    UE_LOG(LogTemp, Warning, TEXT("MyAsset: DONE"));
}
