#pragma once
#include "CoreMinimal.h"
#include "InternalDataStructure.h"
#include "GameFramework/SaveGame.h" // USaveGame
#include "Engine/DataAsset.h"   // UDataAsset
#include "SceneManagementAsset.generated.h"

UCLASS()
class ULightSolutionData : public UObject
{
    GENERATED_BODY()
public:
};


/**
 * Scene Manager Asset Type
 */
UCLASS()
class USceneManagementAsset : public USaveGame
{
    GENERATED_BODY()
public:
    void AddLightingSolution();
    void RemoveLightingSolution(int SolutionIndex);
    void RenameLightingSolution(int SolutionIndex, const FString& SolutionName);
    void SetKeyLightActorName(int SolutionIndex, const FString& ActorName);

public:
    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<FString> KeyLightActorNames;

    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<ULightParams*> KeyLightParams;

    UPROPERTY(EditAnywhere, Category = "Scene Lighting")
        TArray<ULightParamsArray*> SceneAuxLightArrayParams;

    UPROPERTY(EditAnywhere, Category = "Character Lighting")
        TArray<ULightParamsArray*> CharacterAuxLightArrayParams;

private:
    TArray<AActor *> KeyLightActors;
};
