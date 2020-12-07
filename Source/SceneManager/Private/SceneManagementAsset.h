#pragma once
#include "CoreMinimal.h"
#include "InternalDataStructure.h"
#include "GameFramework/SaveGame.h" // USaveGame
#include "Engine/DataAsset.h"   // UDataAsset
#include "SceneManagementAsset.generated.h"

enum ELightCategory
{
    KeyLight = 1,
    AuxLight = 2, 
    SceneLight = 4,
    CharacterLight = 8,
};


/**
 * Scene Manager Asset Type
 */
UCLASS()
class USceneManagementAsset : public UObject
{
    GENERATED_BODY()
public:
    void AddLightingSolution();
    void RemoveLightingSolution(int SolutionIndex);
    void RenameLightingSolution(int SolutionIndex, const FString& SolutionName);

    ULightParams* GetKeyLightParamsPtr(int SolutionIndex);
    ULightParams* GetAuxLightParamsPtr(int SolutionIndex, int LightIndex, ELightCategory LightCategory);
    
    void SyncActorByName();

public:
    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Lighting")
        TArray<ULightParams*> KeyLightParams;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Scene Lighting")
        TArray<UGroupLightParamsArray*> SceneAuxGroups;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Character Lighting")
        TArray<UGroupLightParamsArray*> CharacterAuxGroups;
};
