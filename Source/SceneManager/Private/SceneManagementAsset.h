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
    FString GetKeyLightActorName(int SolutionIndex);
    ULightParams* GetKeyLightParamsPtr(int SolutionIndex);
    
    void AddCharacterAuxLight(int SolutionIndex);
    void SetCharacterAuxLightName(int SolutionIndex, int ActorIndex, const FString& ActorName);
    void GetCharacterAuxLightName(int SolutionIndex, const FString& ActorName);

    void FindAllActors();

public:
    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<FString> KeyLightActorNames;

    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<ULightParams*> KeyLightParams;

    UPROPERTY(EditAnywhere, Category = "Scene Lighting")
        TArray<UStringArray*> SceneAuxLightNames;

    UPROPERTY(EditAnywhere, Category = "Scene Lighting")
        TArray<ULightParamsArray*> SceneAuxLightParams;

    UPROPERTY(EditAnywhere, Category = "Character Lighting")
        TArray<UStringArray*> CharacterAuxLightNames;

    UPROPERTY(EditAnywhere, Category = "Character Lighting")
        TArray<ULightParamsArray*> CharacterAuxLightParams;

private:
    TArray<AActor *> KeyLightActors;
};
