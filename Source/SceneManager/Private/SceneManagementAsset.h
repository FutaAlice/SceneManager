#pragma once
#include "CoreMinimal.h"
#include "SceneManagementAsset.generated.h"

/**
 * Scene Manager Asset Type
 */
UCLASS()
class USceneManagementAsset : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Custom Asset")
    FString Name;
};
