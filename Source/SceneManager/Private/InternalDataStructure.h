#pragma once
#include "CoreMinimal.h"

#include "InternalDataStructure.generated.h"

UCLASS()
class ULightSettings : public UObject
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
