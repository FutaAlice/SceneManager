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
    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(EditAnywhere, Category = "Lighting")
        TArray<ULightParams *> KeyLighList;

    UPROPERTY(EditAnywhere, Category = "Scene Lighting")
        TArray<ULightParamsArray *> AuxLighList;
};
