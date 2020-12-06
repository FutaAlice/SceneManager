#include "SceneManagementAsset.h"

#include "Editor.h" // GEditor
#include "Engine/Level.h"   // ULevel
#include "Engine/Light.h"   // ALight
#include "Engine/World.h"   // UWorld
#include "Kismet/GameplayStatics.h" // UGameplayStatics

void USceneManagementAsset::AddLightingSolution()
{
    LightingSolutionNameList.Add("");
    KeyLightActorNames.Add("");
    KeyLightParams.Add(NewObject<ULightParams>());
    SceneAuxLightArrayParams.Add(NewObject<ULightParamsArray>());
    CharacterAuxLightArrayParams.Add(NewObject<ULightParamsArray>());

    KeyLightActors.Add(nullptr);
    FindAllActors();
}

void USceneManagementAsset::RemoveLightingSolution(int SolutionIndex)
{
    LightingSolutionNameList.RemoveAt(SolutionIndex);
    KeyLightActorNames.RemoveAt(SolutionIndex);
    KeyLightParams.RemoveAt(SolutionIndex);
    SceneAuxLightArrayParams.RemoveAt(SolutionIndex);
    CharacterAuxLightArrayParams.RemoveAt(SolutionIndex);

    KeyLightActors.RemoveAt(SolutionIndex);
}

void USceneManagementAsset::RenameLightingSolution(int SolutionIndex, const FString& SolutionName)
{
    LightingSolutionNameList[SolutionIndex] = SolutionName;
}

void USceneManagementAsset::SetKeyLightActorName(int SolutionIndex, const FString& ActorName)
{
    KeyLightActorNames[SolutionIndex] = ActorName;
    FindAllActors();
}

FString USceneManagementAsset::GetKeyLightActorName(int SolutionIndex)
{
    return KeyLightActorNames[SolutionIndex];
}

ULightParams* USceneManagementAsset::GetKeyLightParamsPtr(int SolutionIndex)
{
    return KeyLightParams[SolutionIndex];
}

void USceneManagementAsset::FindAllActors()
{
    TArray<AActor*> ActorList;
    TArray<FString> ActorNameList;
    {
        UWorld* World = GEditor->GetEditorWorldContext().World();
        ULevel* Level = World->GetCurrentLevel();
        UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);
        for (auto Actor : ActorList) {
            ActorNameList.Add(Actor->GetName());
        }
    }

    for (int Index = 0; Index < LightingSolutionNameList.Num(); ++Index) {
        for (auto ActorName : KeyLightActorNames) {
            FString RecordName = KeyLightActorNames[Index];
            int FoundIndex = ActorNameList.Find(RecordName);
            AActor* FoundActor = nullptr;
            if (FoundIndex != INDEX_NONE) {
                FoundActor = ActorList[FoundIndex];
            } 
            KeyLightActors[Index] = FoundActor;
        }
    }
}
