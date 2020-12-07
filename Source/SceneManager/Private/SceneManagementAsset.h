#pragma once
#include "CoreMinimal.h"
#include "InternalDataStructure.h"
#include "GameFramework/SaveGame.h" // USaveGame
#include "Engine/DataAsset.h"   // UDataAsset
#include "SceneManagementAsset.generated.h"

enum ELightCategory
{
    LightCategory_KeyLight = 1,
    LightCategory_AuxLight = 2,
    LightCategory_SceneLight = 4,
    LightCategory_CharacterLight = 8,
};

UCLASS()
class UGroupLightParams : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "LightParams")
        TArray<ULightParams*> Array;

    ULightParams* AddLightParam();
    void RemoveLightParam(ULightParams *LightParams);
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
    void DuplicateLightingSolution(int SolutionIndex);
    void RemoveLightingSolution(int SolutionIndex);
    void RenameLightingSolution(int SolutionIndex, const FString& SolutionName);

    ULightParams* GetKeyLightParamsPtr(int SolutionIndex);
    UGroupLightParams* GetAuxLightGroupsPtr(int SolutionIndex, ELightCategory LightCategory);
    
    void SyncActorByName();

public:
    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Lighting")
        TArray<ULightParams*> KeyLightParams;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Scene Lighting")
        TArray<UGroupLightParams*> SceneAuxGroups;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Character Lighting")
        TArray<UGroupLightParams*> CharacterAuxGroups;
};
