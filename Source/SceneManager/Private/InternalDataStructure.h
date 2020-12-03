#pragma once
#include "CoreMinimal.h"

#include "InternalDataStructure.generated.h"

UCLASS()
class ULightParams : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FRotator Rotation;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        float Intensity;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FLinearColor LightColor;
};
