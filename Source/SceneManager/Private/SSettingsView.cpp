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
                    for (auto String : OutFilenames) {
                        UE_LOG(LogTemp, Warning, TEXT("SaveFileDialog: %s"), *String);
                    }


                    // Create Package
                    FString FileName = "FUCK";
                    FString pathPackage = FString("/Game/MyTextures/") + FileName;


                    if (SelectedUXXX && SelectedUXXX->MyAsset) {

                        UPackage * Package = CreatePackage(nullptr, *pathPackage);
                        UObject* NewObj = NewObject<UTexture2D>(Package, SelectedUXXX->MyAsset->GetFName(), RF_Public | RF_Standalone);
                        USceneManagementAsset* NewAss = Cast<USceneManagementAsset>(NewObj);
                        Package->SetDirtyFlag(true);

                        FAssetToolsModule &AssetToolModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
                        IAssetTools &AssetTools = AssetToolModule.Get();
                        // AssetTools.DuplicateAsset("FUCK.uasset", "./Game/MyTextures/", SelectedUXXX->MyAsset);

                        TArray<UObject*> SHIT;
                        SHIT.Add(NewAss);
                        AssetTools.ExportAssets(SHIT, "/Game/MyTextures/");
                        
                    }


                    // GEditor->SavePackage(SelectedUXXX, GEngine->GetWorld(), RF_Standalone, *OutFilenames[0]);
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
