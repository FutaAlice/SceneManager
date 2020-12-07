#include "SLightActorDetailPanel.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule

#include "InternalDataStructure.h"
#include "EventHub.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightActorDetailPanel::Construct(const FArguments& InArgs)
{
    //ULightParams *fawef = NewObject<ULightParams>();
    //fawef->AddToRoot();

    //// Debug
    //EventHub::Get();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightViewRef->OnFinishedChangingProperties().AddRaw(this, &SLightActorDetailPanel::OnFinishedChangingProperties);

    // PlayerLightViewRef->SetObject(fawef);
    ChildSlot
    [
        PlayerLightViewRef
    ];

    PlayerLightView = PlayerLightViewRef.operator->();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLightActorDetailPanel::SetActor(AActor *InActor)
{
    if (!InActor || !LightParams) {
        return;
    }

    ALight* InLight = Cast<ALight>(InActor);
    if (InLight == LightParams->LightActor) {
        return; // nothing changed
    }

    LightParams->LightActor = InLight;
    LightParams->FromActor();
}


void SLightActorDetailPanel::BindDataField(UObject* InObject)
{
    LightParams = Cast<ULightParams>(InObject);
    PlayerLightView->SetObject(InObject);
}

void SLightActorDetailPanel::ForceRefresh()
{
    PlayerLightView->ForceRefresh();
}

ULightParams *SLightActorDetailPanel::GetParam()
{
    return LightParams;
}

void SLightActorDetailPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (LightParams->LightActor) {
        LightParams->ToActor();
    }
    else {
    }
}
