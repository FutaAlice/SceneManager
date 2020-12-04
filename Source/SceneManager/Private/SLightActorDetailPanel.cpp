// Fill out your copyright notice in the Description page of Project Settings.


#include "SLightActorDetailPanel.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule

#include "InternalDataStructure.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightActorDetailPanel::Construct(const FArguments& InArgs)
{
    LightParams = NewObject<ULightParams>();
    LightParams->AddToRoot();
    Light = nullptr;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightView->SetObject(LightParams);
    PlayerLightView->OnFinishedChangingProperties().AddRaw(this, &SLightActorDetailPanel::OnFinishedChangingProperties);
    ChildSlot
    [
        PlayerLightView
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SLightActorDetailPanel::~SLightActorDetailPanel()
{
    UE_LOG(LogTemp, Warning, TEXT("SLightActorDetailPanel Destory"));
}

void SLightActorDetailPanel::BindActor(AActor *InActor)
{
    ensure(InActor);

    if (Light == InActor) {
        return; // nothing changed
    }

    Light = InActor;
    LightParams->FromActor(InActor);
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
