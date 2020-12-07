#include "SMPCDetialsPanel.h"

#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Materials/MaterialParameterCollection.h"	// UMaterialParameterCollection

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMPCDetialsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMPCDetialsPanel::OnFinishedChangingProperties);

    ChildSlot
    [
        DetailsViewRef
    ];

    DetailsView = DetailsViewRef.operator->();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMPCDetialsPanel::SetObject(UObject* InObject)
{
    DetailsView->SetObject(InObject);
}

void SMPCDetialsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
}
