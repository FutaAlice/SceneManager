#include "InternalDataStructure.h"

#include "Engine/Light.h"   // ALight
#include "Components/LightComponent.h"  // ULightComponent

void ULightParams::Clear()
{
    Rotation = FRotator();
    Intensity = 0;
    LightColor = FLinearColor();
}

void ULightParams::FromActor(AActor *Actor)
{
    if (ALight *Light = Cast<ALight>(Actor)) {
        Rotation = Light->GetLightComponent()->GetComponentRotation();
        Intensity = Light->GetLightComponent()->Intensity;
        LightColor = Light->GetLightColor();
    }
}

void ULightParams::ToActor(AActor *Actor)
{
    if (ALight *Light = Cast<ALight>(Actor)) {
        Light->GetLightComponent()->SetWorldRotation(Rotation);
        Light->GetLightComponent()->SetIntensity(Intensity);
        Light->SetLightColor(LightColor);
    }
}
