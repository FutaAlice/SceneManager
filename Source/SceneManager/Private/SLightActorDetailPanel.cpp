// Fill out your copyright notice in the Description page of Project Settings.


#include "SLightActorDetailPanel.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Engine/Light.h"   // ALight
#include "Components/LightComponent.h"  // ULightComponent

#include "InternalDataStructure.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightActorDetailPanel::Construct(const FArguments& InArgs)
{
    LightParams = NewObject<ULightParams>();
    LightParams->AddToRoot();

    BindActor(nullptr);
    SetParam(nullptr);

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

bool SLightActorDetailPanel::BindActor(ALight *InActor)
{
    if (InActor) {
        if (Light == InActor) {
            return true;    // nothing changed
        }
        Light = InActor;
        // Light->OnDestroyed.AddDynamic(this, &SLightActorDetailPanel::OnLightDestroyed);
        ULightParams *InData = NewObject<ULightParams>();
        InData->Rotation = InActor->GetLightComponent()->GetComponentRotation();
        InData->Intensity = InActor->GetLightComponent()->Intensity;
        InData->LightColor = InActor->GetLightColor();
        return SetParam(InData);
    }
    else {
        return SetParam(nullptr);
    }
}

bool SLightActorDetailPanel::SetParam(ULightParams *InData)
{
    if (InData) {
        LightParams->Rotation = InData->Rotation;
        LightParams->Intensity = InData->Intensity;
        LightParams->LightColor = InData->LightColor;
        return true;
    }
    else {
        LightParams->Rotation = FRotator();
        LightParams->Intensity = 0;
        LightParams->LightColor = FLinearColor();
        return true;
    }
}

ULightParams *SLightActorDetailPanel::GetParam()
{
    return Light ? LightParams : nullptr;
}

void SLightActorDetailPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (Light) {
        Light->GetLightComponent()->SetWorldRotation(LightParams->Rotation);
        Light->GetLightComponent()->SetIntensity(LightParams->Intensity);
        Light->SetLightColor(LightParams->LightColor);
    }
    else {
        SetParam(nullptr);
    }
}

void SLightActorDetailPanel::OnLightDestroyed(AActor *)
{
    Light = nullptr;
    SetParam(nullptr);
}
