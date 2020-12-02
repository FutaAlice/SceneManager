#pragma once
#include "CoreMinimal.h"

#include "SSceneLightingViewer.generated.h"

class FTabManager;

namespace SceneLightingViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace SceneLightingViewer

UCLASS()
class UPlayerLightSettings : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FRotator rotation;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        float intensity;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FLinearColor color;
};

