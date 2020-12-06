#include "SceneManagementAsset.h"

void USceneManagementAsset::AddLightingSolution()
{
    LightingSolutionNameList.Add("");
    KeyLightActorNames.Add("");
    KeyLightParams.Add(NewObject<ULightParams>());
    SceneAuxLightArrayParams.Add(NewObject<ULightParamsArray>());
    CharacterAuxLightArrayParams.Add(NewObject<ULightParamsArray>());

    KeyLightActors.Add(nullptr);
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
}
