#include "SMaterialDetailsPanel.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule

#include "SceneManagementAssetData.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialDetailsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> PlayerLightViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PlayerLightViewRef->OnFinishedChangingProperties().AddRaw(this, &SMaterialDetailsPanel::OnFinishedChangingProperties);

    ChildSlot
    [
        PlayerLightViewRef
    ];

    DetailView = PlayerLightViewRef.operator->();
}
void SMaterialDetailsPanel::BindDataField(UObject* InObject)
{
    MaterialInfo = Cast<UMaterialInfo>(InObject);
    DetailView->SetObject(InObject);
}
UMaterialInfo* SMaterialDetailsPanel::GetDataField()
{
	return nullptr;
}
void SMaterialDetailsPanel::ForceRefresh()
{
}
void SMaterialDetailsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
