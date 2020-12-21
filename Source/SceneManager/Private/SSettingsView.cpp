#include "SSettingsView.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "IDesktopPlatform.h"   // IDesktopPlatform
#include "DesktopPlatformModule.h"  // FDesktopPlatformModule
#include "Misc/Paths.h" // FPahts
#include "HAL/FileManager.h"    // IFileManager
#include "UObject/SoftObjectPath.h" // FSoftObjectPath
#include "Editor.h" // GEditor
#include "AssetToolsModule.h"   // FAssetToolsModule
#include "IAssetTools.h"    // IAssetTools
#include "AssetRegistryModule.h"
#include "FileHelpers.h"    // UEditorLoadingAndSavingUtils
#include "IAssetRegistry.h" // IAssetRegistry
#include "Widgets/SBoxPanel.h"  // SVerticalBox, SHorizontalBox
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Layout/SScrollBox.h"  // SScrollBox
#include "Materials/MaterialParameterCollection.h"  // UMaterialParameterCollection

#include "SceneManagementAssetData.h"
#include "EventHub.h"

SSettingsView *SSettingsView::Instance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSettingsView::Construct(const FArguments& InArgs)
{
    Instance = this;

    // Init AssetWrap
    AssetWrap = NewObject<UAssetWrap>();
    AssetWrap->AddToRoot();

    // Create AssetWrap DetailView widget
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightView->SetObject(AssetWrap);
    PlayerLightView->OnFinishedChangingProperties().AddRaw(this, &SSettingsView::OnSceneManagementAssetChanged);

    TSharedRef<IDetailsView> GroupNameViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    GroupNameView = GroupNameViewRef.operator->();

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .Text(FText::FromString("Save"))
                .OnClicked_Raw(this, &SSettingsView::SaveAssetData)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    PlayerLightView
                ]
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            [
                GroupNameViewRef
            ]
        ]
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SSettingsView::~SSettingsView()
{
    Instance = nullptr;
}

SSettingsView* SSettingsView::Get()
{
    return Instance;
}

void SSettingsView::OnSceneManagementAssetChanged(const FPropertyChangedEvent& InEvent)
{
    FString PropertyName = InEvent.GetPropertyName().ToString();
    // AssetData changed
    if (PropertyName == "AssetData") {
        USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected(false);
        EventHub::Get()->OnAssetDataSelected(AssetData);
        GroupNameView->SetObject(AssetData ? AssetData->EnabledGroupName : nullptr);
    }

    // MPC changed
    if (PropertyName.Contains("MPC")) {
        EMPCOwner Owner = -1;
        UMaterialParameterCollection* MPC = nullptr;
        if (PropertyName.Contains("Scene")) {
            Owner = MPCOwner_ScenLight;
            MPC = Instance->AssetWrap->SceneLightingMPC;
        }
        if (PropertyName.Contains("Character")) {
            Owner = MPCOwner_CharacterLight;
            MPC = Instance->AssetWrap->CharacterLightingMPC;
        }
        if (PropertyName.Contains("Wind")) {
            Owner = MPCOwner_Wind;
            MPC = Instance->AssetWrap->WindMPC;
        }
        if (PropertyName.Contains("Post")) {
            Owner = MPCOwner_PostProc;
            MPC = Instance->AssetWrap->PostProcessingMPC;
        }
        ensure(Owner >= 0);

        EventHub::Get()->OnMPCSelected(MPC, Owner);
    }
}

FReply SSettingsView::SaveAssetData()
{
    if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
        // FSoftObjectPath
        FStringAssetReference StringAssetReference(AssetData);
        FString AssetPath = StringAssetReference.GetAssetPathString();  // /Game/NewSceneManagementAsset.NewSceneManagementAsset
        FString BaseFilePath = FPaths::GetBaseFilename(AssetPath, false);   // /Game/NewSceneManagementAsset
        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath AssetPath: %s"), *AssetPath);
        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath BaseFilePath: %s"), *BaseFilePath);
        UE_LOG(LogTemp, Warning, TEXT("ProjectContentDir: %s"), *FPaths::ProjectContentDir());  //  D:/Unreal Projects/SMRefactor/Content/

        TArray<UPackage*> Packages;
        UPackage* Outermost = AssetData->GetOutermost();
        // Fully load and check out is done in UEditorLoadingAndSavingUtils::SavePackages
        Packages.Add(Outermost);
        UEditorLoadingAndSavingUtils::SavePackages(Packages, false);
    }
    return FReply::Handled();
}
