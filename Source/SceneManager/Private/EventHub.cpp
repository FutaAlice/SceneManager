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

std::shared_ptr<EventHub> EventHub::_instance;
std::mutex EventHub::_mutex;

EventHub::EventHub()
{
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

    //// Save 
    //FCoreUObjectDelegates::OnObjectSaved.AddLambda([](UObject *) {
    //    UE_LOG(LogTemp, Warning, TEXT("OnObjectSaved"));
    //});

    // PropertyChanged
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject*, struct FPropertyChangedEvent &) {
        UE_LOG(LogTemp, Warning, TEXT("OnObjectPropertyChanged"));
    });

    // Modified (what's the different from 'OnObjectPropertyChanged'£¿)
    FCoreUObjectDelegates::OnObjectModified.AddLambda([](UObject*) {
        UE_LOG(LogTemp, Warning, TEXT("OnObjectModified"));
    });

    //FEditorDelegates::MapChange.AddLambda([]() {
    //    UE_LOG(LogTemp, Warning, TEXT("MapChange"));
    //});

    // NOT WORK
    FEditorDelegates::ActorPropertiesChange.AddLambda([]() {
        UE_LOG(LogTemp, Warning, TEXT("ActorPropertiesChange"));
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
