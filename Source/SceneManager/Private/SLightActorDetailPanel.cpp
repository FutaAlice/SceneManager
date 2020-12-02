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
    FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true);
    DetailsViewArgs.bAllowSearch = false;
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    TSharedRef<IDetailsView> PlayerLightView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    // PlayerLightView->OnFinishedChangingProperties().AddRaw(this, &SPlayerLightManager::OnFinishedChangingMainLight);
    LightSettings = MakeShareable(NewObject<ULightSettings>());
    PlayerLightView->SetObject(LightSettings.Get());

    ChildSlot
    [
        PlayerLightView
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
