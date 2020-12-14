#include "EventHub.h"
#include "Editor.h" // FEditorDelegates
#include "Engine/Selection.h"   // USelection
#include "IPropertyChangeListener.h"    // FPropertyListenerSettings
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Components/SceneComponent.h"  // USceneComponent
#include "Engine/Light.h"   // ALight
#include "Components/LightComponent.h"  // ULightComponent
#include "Misc/CoreDelegates.h" // FCoreDelegates
#include "UObject/UObjectGlobals.h" // FCoreUObjectDelegates
#include "Engine/World.h"   // FWorldDelegates

#include "SceneManagementAssetData.h"
#include "SLightingViewer.h"
#include "SMaterialViewer.h"

std::shared_ptr<EventHub> EventHub::_instance;
std::mutex EventHub::_mutex;

EventHub::EventHub()
{
    // manually call asset clean up when change map
    FWorldDelegates::OnWorldCleanup.AddLambda([](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
        UE_LOG(LogTemp, Warning, TEXT("OnWorldCleanup: %s"), *World->GetName());
        USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
        if (AssetData) {
            AssetData->CleanUp();
        }
    });

    //// PropertyChanged
    //FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject*, struct FPropertyChangedEvent &) {
    //    UE_LOG(LogTemp, Warning, TEXT("OnObjectPropertyChanged"));
    //});

    // sync asset when editor modified (what's the different from 'OnObjectPropertyChanged'£¿)
    FCoreUObjectDelegates::OnObjectModified.AddLambda([](UObject* InObject) {


        if (InObject->IsA(ALight::StaticClass()) || InObject->IsA(ULightComponent::StaticClass())) {
            if (USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected(false)) {
                AssetData->SyncDataByActor();
            }
        }
        else {
            static TSet<FString> Fuck{
                "MaterialEditorInstanceConstant",
                "DEditorScalarParameterValue",
                "DEditorVectorParameterValue",
                "DEditorTextureParameterValue",
                "DEditorStaticSwitchParameterValue",
                "MaterialExpressionScalarParameter",
                "MaterialExpressionVectorParameter",
            };

            UClass *InClass = InObject->GetClass();
            if (Fuck.Contains(InClass->GetName())) {
                UE_LOG(LogTemp, Warning, TEXT("Material modified"));
            }

        }
    });

    // Delete
    FEditorDelegates::OnDeleteActorsEnd.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("OnDeleteActorsEnd"));
    });

    // Ctrl + X
    FEditorDelegates::OnEditCutActorsEnd.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("OnEditCutActorsEnd"));
    });

    // Ctrl + C
    FEditorDelegates::OnEditCopyActorsEnd.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("OnEditCopyActorsEnd"));
    });

    // Ctrl + V
    FEditorDelegates::OnEditPasteActorsEnd.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("OnEditPasteActorsEnd"));
    });

    // Ctrl + W
    FEditorDelegates::OnDuplicateActorsEnd.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("OnDuplicateActorsEnd"));
    });

    // RENAME
    FCoreDelegates::OnActorLabelChanged.AddLambda([](AActor *Actor) {
        UE_LOG(LogTemp, Warning, TEXT("OnActorLabelChanged"));
    });

    // Save 
    FCoreUObjectDelegates::OnObjectSaved.AddLambda([](UObject *Object) {
        FString ObjectName = Object->GetClass()->GetFName().ToString();
        UE_LOG(LogTemp, Warning, TEXT("OnObjectSaved: %s"), *ObjectName);
    });
}

EventHub::~EventHub()
{
}

std::shared_ptr<EventHub> EventHub::Get()
{
    // "double checked lock"
    if (_instance == nullptr) {
        std::lock_guard<std::mutex> lk(_mutex);
        if (_instance == nullptr) {
            _instance = std::shared_ptr<EventHub>(new EventHub());
        }
    }
    return _instance;
}

void EventHub::OnAssetDataSelected(USceneManagementAssetData * AssetData)
{
    // post initialize while input asset not empty
    if (AssetData) {
        AssetData->SyncActorByName();
    }
    LightingViewer::OnAssetDataChanged(AssetData);
    MaterialViewer::OnAssetDataChanged(AssetData);
}

void EventHub::OnMPCSelected(UMaterialParameterCollection * MPC, EMPCOwner Owner)
{
    switch (Owner) {
    case MPCOwner_ScenLight:
        LightingViewer::OnMPCChanged(MPC, LightCategory_SceneLight);
        break;
    case MPCOwner_CharacterLight:
        LightingViewer::OnMPCChanged(MPC, LightCategory_CharacterLight);
        break;


    default:
        break;
    }
}
