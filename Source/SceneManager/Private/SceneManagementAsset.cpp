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

    SceneAuxLightNames.Add(NewObject<UStringArray>());
    SceneAuxLightParams.Add(NewObject<ULightParamsArray>());

    CharacterAuxLightNames.Add(NewObject<UStringArray>());
    CharacterAuxLightParams.Add(NewObject<ULightParamsArray>());

    KeyLightActors.Add(nullptr);
    FindAllActors();
}

void USceneManagementAsset::RemoveLightingSolution(int SolutionIndex)
{
    LightingSolutionNameList.RemoveAt(SolutionIndex);
    KeyLightActorNames.RemoveAt(SolutionIndex);
    KeyLightParams.RemoveAt(SolutionIndex);
    SceneAuxLightNames.RemoveAt(SolutionIndex);
    SceneAuxLightParams.RemoveAt(SolutionIndex);
    SceneAuxLightNames.RemoveAt(SolutionIndex);
    SceneAuxLightParams.RemoveAt(SolutionIndex);

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

void USceneManagementAsset::AddCharacterAuxLight(int SolutionIndex)
{
    CharacterAuxLightNames[SolutionIndex]->Array.Add("");
    CharacterAuxLightParams[SolutionIndex]->Array.Add(NewObject<ULightParams>());
}

void USceneManagementAsset::SetCharacterAuxLightName(int SolutionIndex, int ActorIndex, const FString& ActorName)
{
    CharacterAuxLightNames[SolutionIndex]->Array[ActorIndex] = ActorName;
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

    if (KeyLightActors.Num() != LightingSolutionNameList.Num()) {
        KeyLightActors.Init(nullptr, LightingSolutionNameList.Num());
    }
    for (int Index = 0; Index < LightingSolutionNameList.Num(); ++Index) {
        for (auto ActorName : KeyLightActorNames) {
            FString RecordName = KeyLightActorNames[Index];
            int FoundIndex = ActorNameList.Find(RecordName);
            AActor* FoundActor = nullptr;
            if (FoundIndex >= 0) {
                FoundActor = ActorList[FoundIndex];
            }
            KeyLightActors[Index] = FoundActor;
        }
    }
}
