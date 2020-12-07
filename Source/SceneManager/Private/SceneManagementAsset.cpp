#include "SceneManagementAsset.h"

#include "Editor.h" // GEditor
#include "Engine/Level.h"   // ULevel
#include "Engine/Light.h"   // ALight
#include "Engine/World.h"   // UWorld
#include "Kismet/GameplayStatics.h" // UGameplayStatics

void USceneManagementAsset::AddLightingSolution()
{
    LightingSolutionNameList.Add("");

    // Attach to root object
    KeyLightParams.Add(NewObject<ULightParams>(this));
    SceneAuxGroups.Add(NewObject<UGroupLightParamsArray>(this));
    CharacterAuxGroups.Add(NewObject<UGroupLightParamsArray>(this));

    SyncActorByName();
}

void USceneManagementAsset::RemoveLightingSolution(int SolutionIndex)
{
    LightingSolutionNameList.RemoveAt(SolutionIndex);
    KeyLightParams.RemoveAt(SolutionIndex);
    SceneAuxGroups.RemoveAt(SolutionIndex);
    CharacterAuxGroups.RemoveAt(SolutionIndex);
}

void USceneManagementAsset::RenameLightingSolution(int SolutionIndex, const FString& SolutionName)
{
    LightingSolutionNameList[SolutionIndex] = SolutionName;
}

ULightParams* USceneManagementAsset::GetKeyLightParamsPtr(int SolutionIndex)
{
    ensure(KeyLightParams.Num() > SolutionIndex);
    return KeyLightParams[SolutionIndex];
}

ULightParams* USceneManagementAsset::GetAuxLightParamsPtr(int SolutionIndex, int LightIndex, ELightCategory LightCategory)
{
    if (LightCategory == (AuxLight | SceneLight)) {
        ensure(SceneAuxGroups.Num() > SolutionIndex);
        const auto& Group = SceneAuxGroups[SolutionIndex];
        ensure(Group->Array.Num() > LightIndex);
        return Group->Array[LightIndex];
    }
    else if (LightCategory == (AuxLight | CharacterLight)) {
        ensure(CharacterAuxGroups.Num() > SolutionIndex);
        const auto& Group = CharacterAuxGroups[SolutionIndex];
        ensure(Group->Array.Num() > LightIndex);
        return Group->Array[LightIndex];
    }
    else {
        return nullptr;
    }
}

void USceneManagementAsset::SyncActorByName()
{
    // gather all ALight actor in level
    TArray<AActor*> ActorList;
    TArray<FString> NameList;
    {
        UWorld* World = GEditor->GetEditorWorldContext().World();
        ULevel* Level = World->GetCurrentLevel();
        UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);
        for (auto Actor : ActorList) {
            NameList.Add(Actor->GetName());
        }
    }

    ensure(LightingSolutionNameList.Num() == KeyLightParams.Num());
    ensure(LightingSolutionNameList.Num() == SceneAuxGroups.Num());
    ensure(LightingSolutionNameList.Num() == CharacterAuxGroups.Num());

    auto SyncActor = [ActorList, NameList](ULightParams* LightParams) {
        // TODO: why??
        if (!LightParams) {
            LightParams = NewObject<ULightParams>();
        }

        FString ActorName = LightParams->ActorName;
        AActor* LightActor = nullptr;
        do {
            if (ActorName.IsEmpty()) {
                break;
            }
            int FoundIndex = NameList.Find(ActorName);
            if (FoundIndex < 0) {
                break;
            }
            LightActor = ActorList[FoundIndex];
        } while (0);
        LightParams->LightActor = Cast<ALight>(LightActor);
    };

    // key light
    for (ULightParams *LightParams : KeyLightParams) {
        SyncActor(LightParams);
    }

    // TODO
    // Aux Light
    //for (auto LightGroup : SceneAuxGroups) {
    //    for (ULightParams* LightParams : LightGroup->Array) {
    //        SyncActor(LightParams);
    //    }
    //}
    //for (auto LightGroup : CharacterAuxGroups) {
    //    for (ULightParams* LightParams : LightGroup->Array) {
    //        SyncActor(LightParams);
    //    }
    //}
}
