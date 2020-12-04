#pragma once
#include "CoreMinimal.h"

#include "InternalDataStructure.generated.h"

class AActor;

UCLASS()
class ULightParams : public UObject
{
    GENERATED_BODY()
public:
    ~ULightParams()
    {
        // TODO: FIX MEMORY LEAK
        // GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "SHIT");
    }

    void Clear();
    void FromActor(AActor *Actor);
    void ToActor(AActor *Actor);
    
    
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FRotator Rotation;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        float Intensity;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FLinearColor LightColor;
};

UCLASS()
class ULightParamsArray : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "LightParams")
        TArray<ULightParams *> Array;
};
