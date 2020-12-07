#include "SceneManagementAsset.h"

#include "Editor.h" // GEditor
#include "Engine/Level.h"   // ULevel
#include "Engine/Light.h"   // ALight
#include "Engine/World.h"   // UWorld
#include "Kismet/GameplayStatics.h" // UGameplayStatics

ULightParams* UGroupLightParams::AddLightParam()
{
    ULightParams *LightParams = NewObject<ULightParams>(this);
    Array.Add(LightParams);
    return LightParams;
}

void UGroupLightParams::RemoveLightParam(ULightParams* LightParams)
{
    int Index = Array.Find(LightParams);
    ensure(Index >= 0);
    Array.RemoveAt(Index);
}

void USceneManagementAsset::AddLightingSolution()
{
    LightingSolutionNameList.Add("");

    // Attach to root object
    KeyLightParams.Add(NewObject<ULightParams>(this));
    SceneAuxGroups.Add(NewObject<UGroupLightParams>(this));
    CharacterAuxGroups.Add(NewObject<UGroupLightParams>(this));

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

UGroupLightParams* USceneManagementAsset::GetAuxLightGroupsPtr(int SolutionIndex, ELightCategory LightCategory)
{
    TArray<UGroupLightParams*>* GroupArray = nullptr;
    if (LightCategory & LightCategory_SceneLight) {
        GroupArray = &SceneAuxGroups;
    }
    else if (LightCategory & LightCategory_CharacterLight) {
        GroupArray = &CharacterAuxGroups;
    }

    ensure(GroupArray);
    ensure(GroupArray->Num() > SolutionIndex);
    return (*GroupArray)[SolutionIndex];
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

    // Aux Light
    for (auto LightGroup : SceneAuxGroups) {
        for (ULightParams* LightParams : LightGroup->Array) {
            SyncActor(LightParams);
        }
    }
    for (auto LightGroup : CharacterAuxGroups) {
        for (ULightParams* LightParams : LightGroup->Array) {
            SyncActor(LightParams);
        }
    }
}
