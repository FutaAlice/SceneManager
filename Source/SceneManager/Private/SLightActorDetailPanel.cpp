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
    //LightParams = NewObject<ULightParams>();
    //LightParams->AddToRoot();
    //Light = nullptr;

    //// Debug
    //EventHub::Get();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightViewRef->OnFinishedChangingProperties().AddRaw(this, &SLightActorDetailPanel::OnFinishedChangingProperties);

    // PlayerLightViewRef->SetObject(nullptr);
    ChildSlot
    [
        PlayerLightViewRef
    ];

    PlayerLightView = PlayerLightViewRef.operator->();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLightActorDetailPanel::BindActor(AActor *InActor)
{
    ensure(InActor);

    if (Light == InActor) {
        return; // nothing changed
    }

    Light = InActor;

    if (LightParams) {
        LightParams->FromActor(InActor);
    }
}


void SLightActorDetailPanel::SetObject(UObject* InObject)
{
    PlayerLightView->SetObject(InObject);
    LightParams = Cast<ULightParams>(InObject);
    LightParams->FromActor(Light);
}

ULightParams *SLightActorDetailPanel::GetParam()
{
    return Light ? LightParams : nullptr;
}

void SLightActorDetailPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (Light) {
        LightParams->ToActor(Light);
    }
    else {
        
    }
}
