#include "SSettingsView.h"
#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "IDesktopPlatform.h"   // IDesktopPlatform
#include "DesktopPlatformModule.h"  // FDesktopPlatformModule
#include "Misc/Paths.h" // FPahts
#include "HAL/FileManager.h"    // IFileManager

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
                        FString OuterName = SelectedUXXX->MyAsset->GetOuter()->GetName();
                        FString OuterPath = FPaths::GetPath(OuterName);
                        UE_LOG(LogTemp, Warning, TEXT("OuterName: %s"), *OuterName);
                        UE_LOG(LogTemp, Warning, TEXT("OuterPath: %s"), *OuterPath);

                        // "/Game/NewSceneManagementAsset"
                        const FString& RelPath = FPaths::ProjectDir();
                        const FString& FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*OuterPath);
                        UE_LOG(LogTemp, Warning, TEXT("RelPath: %s"), *RelPath);
                        UE_LOG(LogTemp, Warning, TEXT("FullPath: %s"), *FullPath);
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
                    FString DefaultPath = "";
                    FString DefaultFile = "";
                    FString FileTypes = "uasset";
                    uint32 Flags = 0; 
                    if (SelectedUXXX) {
                        DefaultPath = SelectedUXXX->GetPathName();
                    }
                    FDesktopPlatformModule::Get()->SaveFileDialog(nullptr, DialogTitle, DefaultPath, DefaultFile, FileTypes, Flags, OutFilenames);
                    for (auto String : OutFilenames) {
                        UE_LOG(LogTemp, Warning, TEXT("SaveFileDialog: %s"), *String);
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
