#include "EventHub.h"
#include "Editor.h" // FEditorDelegates
#include "Engine/Selection.h"   // USelection
#include "IPropertyChangeListener.h"    // FPropertyListenerSettings
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Components/SceneComponent.h"  // USceneComponent
#include "Engine/Light.h"   // ALight
#include "Components/LightComponent.h"  // ULightComponent

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

    /*
    FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    static TSharedPtr<IPropertyChangeListener> PropertyChangeListener = PropertyEditor.CreatePropertyChangeListener();
    PropertyChangeListener->GetOnPropertyChangedDelegate().AddLambda(
        [](const TArray<UObject*>& ChangedObjects, const IPropertyHandle& PropertyHandle) {
            UE_LOG(LogTemp, Warning, TEXT("PropertyChangeListener"));
        });

    FPropertyListenerSettings Settings;
    Settings.bIgnoreArrayProperties = false;
    Settings.bIgnoreObjectProperties = false;
    // Property flags which must be on the property
    Settings.RequiredPropertyFlags = CPF_Edit; // or CPF_Edit
    // Property flags which cannot be on the property
    Settings.DisallowedPropertyFlags = CPF_None;


    // Select (or SelectObjectEvent/SelectNoneEvent)
    static AActor *SelectedActor = nullptr;
    USelection::SelectionChangedEvent.AddLambda([&](UObject *Object) {
        AActor* OldSelectedActor = SelectedActor;
        SelectedActor = GEditor->GetSelectedActors()->GetBottom<AActor>();

        //if (USelection* Selection = Cast<USelection>(Object)) {
        //    for (int32 Idx = 0; Idx < Selection->Num(); Idx++) {
        //        UObject *SelectedObject = Selection->GetSelectedObject(Idx);
        //        UE_LOG(LogTemp, Warning, TEXT("SelectionChangedEvent: %s"), *SelectedObject->GetName());
        //    }

        //}

        if (SelectedActor && SelectedActor != OldSelectedActor) {
            UE_LOG(LogTemp, Warning, TEXT("SelectionChangedEvent: %s"), *SelectedActor->GetName());
            if (ALight *Light = Cast<ALight>(SelectedActor)) {
                UE_LOG(LogTemp, Warning, TEXT("Light: %s"), *Light->GetName());
                PropertyChangeListener->SetObject(*Light->GetLightComponent(), Settings);
                PropertyChangeListener->TriggerAllPropertiesChangedDelegate();

            }
        }
    });
    */

    //FEditorDelegates::MapChange.AddLambda([]() {
    //    UE_LOG(LogTemp, Warning, TEXT("MapChange"));
    //});

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
