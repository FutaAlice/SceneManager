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
#include "IAssetRegistry.h" // IAssetRegistry

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
                    if (SelectedUXXX && SelectedUXXX->MyAsset) {
                        // FSoftObjectPath
                        FStringAssetReference StringAssetReference(SelectedUXXX->MyAsset);  
                        FString AssetPath = StringAssetReference.GetAssetPathString();  // /Game/NewSceneManagementAsset.NewSceneManagementAsset
                        FString BaseFilePath = FPaths::GetBaseFilename(AssetPath, false);   // /Game/NewSceneManagementAsset
                        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath AssetPath: %s"), *AssetPath);
                        UE_LOG(LogTemp, Warning, TEXT("FSoftObjectPath BaseFilePath: %s"), *BaseFilePath);
                        UE_LOG(LogTemp, Warning, TEXT("ProjectContentDir: %s"), *FPaths::ProjectContentDir());  //  D:/Unreal Projects/SMRefactor/Content/


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
                    FDesktopPlatformModule::Get()->SaveFileDialog(nullptr, DialogTitle, DefaultPath, DefaultFile, FileTypes, Flags, OutFilenames);

                    FString ContentDir = FPaths::ProjectContentDir();
                    FString RootDir = "/Game/";
                    FString InGamePath = OutFilenames[0].Replace(*ContentDir, *RootDir);

                    FString NewName = FPaths::GetBaseFilename(InGamePath);
                    FString NewPath = FPaths::GetPath(InGamePath);
                    UE_LOG(LogTemp, Warning, TEXT("NewName: %s"), *NewName);
                    UE_LOG(LogTemp, Warning, TEXT("NewPath: %s"), *NewPath);

                    if (SelectedUXXX && SelectedUXXX->MyAsset) {
                        FAssetToolsModule &AssetToolModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
                        IAssetTools &AssetTools = AssetToolModule.Get();
                        UObject *Asset = SelectedUXXX->MyAsset;

                        // TODO: fix RENAME
                        TArray<FAssetRenameData> AssetsAndNames;
                        new(AssetsAndNames) FAssetRenameData(Asset, NewPath, NewName);
                        AssetTools.RenameAssets(AssetsAndNames);
                    }
                    return FReply::Handled();
                })
            ]
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
