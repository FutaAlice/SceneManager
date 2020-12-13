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
    AssetWrap->AssetData = nullptr;
    AssetWrap->AddToRoot();

    // Create AssetWrap DetailView widget
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
    TSharedRef<IDetailsView> PlayerLightView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PlayerLightView->SetObject(AssetWrap);
    PlayerLightView->OnFinishedChangingProperties().AddRaw(this, &SSettingsView::OnSceneManagementAssetChanged);

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            PlayerLightView
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString("Save"))
                .OnClicked_Lambda([this]() -> FReply {
                    if (USceneManagementAssetData *AssetData = USceneManagementAssetData::GetSelected()) {
                        // FSoftObjectPath
                        FStringAssetReference StringAssetReference(AssetData);
                        FString AssetPath = StringAssetReference.GetAssetPathString();  // /Game/NewSceneManagementAsset.NewSceneManagementAsset
                        FString BaseFilePath = FPaths::GetBaseFilename(AssetPath, false);   // /Game/NewSceneManagementAsset
                        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath AssetPath: %s"), *AssetPath);
                        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath BaseFilePath: %s"), *BaseFilePath);
                        UE_LOG(LogTemp, Warning, TEXT("ProjectContentDir: %s"), *FPaths::ProjectContentDir());  //  D:/Unreal Projects/SMRefactor/Content/
                        
                        TArray<UPackage*> Packages;
                        UPackage* Outermost = AssetData->GetOutermost();
                        Packages.Add(Outermost); // Fully load and check out is done in UEditorLoadingAndSavingUtils::SavePackages

                        for (auto Obj : AssetData->KeyLightParams) {

                        }

                        UEditorLoadingAndSavingUtils::SavePackages(Packages, false);

                    }
                    return FReply::Handled();
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString("Save as ..."))
                .OnClicked_Lambda([this]() -> FReply {
                    TArray<FString> OutFilenames;
                    FString DialogTitle = "Save as ...";
                    FString DefaultPath = FPaths::ProjectContentDir();
                    FString DefaultFile = "";
                    FString FileTypes = "uasset files (*.uasset;)|*.uasset;";
                    uint32 Flags = 0;

                    IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
                    DesktopPlatform->SaveFileDialog(nullptr, DialogTitle, DefaultPath, DefaultFile, FileTypes, Flags, OutFilenames);

                    FString ContentDir = FPaths::ProjectContentDir();
                    FString RootDir = "/Game/";
                    FString InGamePath = OutFilenames[0].Replace(*ContentDir, *RootDir);

                    FString NewName = FPaths::GetBaseFilename(InGamePath);
                    FString NewPath = FPaths::GetPath(InGamePath);
                    UE_LOG(LogTemp, Warning, TEXT("NewName: %s"), *NewName);
                    UE_LOG(LogTemp, Warning, TEXT("NewPath: %s"), *NewPath);

                    if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected()) {
                        FAssetToolsModule &AssetToolModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
                        IAssetTools &AssetTools = AssetToolModule.Get();

                        // TODO: fix RENAME
                        TArray<FAssetRenameData> AssetsAndNames;
                        new(AssetsAndNames) FAssetRenameData(AssetData, NewPath, NewName);
                        AssetTools.RenameAssets(AssetsAndNames);
                    }
                    return FReply::Handled();
                })
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
        EventHub::Get()->OnAssetDataSelected(USceneManagementAssetData::GetSelected(false));
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
