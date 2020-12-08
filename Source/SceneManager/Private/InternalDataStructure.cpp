#include "InternalDataStructure.h"

#include "Engine/Light.h"   // ALight
#include "Components/LightComponent.h"  // ULightComponent

void ULightParams::Clear()
{
    LightActor = nullptr;
    ActorName = "";
    Rotation = FRotator();
    Intensity = 0;
    LightColor = FLinearColor();
}

void ULightParams::FromActor()
{
    if (LightActor) {
        ActorName = LightActor->GetName();
        // Rotation = LightActor->GetLightComponent()->GetComponentRotation();
        Rotation = LightActor->GetActorRotation();
        Intensity = LightActor->GetLightComponent()->Intensity;
        LightColor = LightActor->GetLightColor();
    }
}

void ULightParams::ToActor()
{
    if (LightActor) {
        //LightActor->GetLightComponent()->SetWorldRotation(Rotation);
        LightActor->SetActorRotation(Rotation);
        LightActor->GetLightComponent()->SetIntensity(Intensity);
        LightActor->SetLightColor(LightColor);
    }
}
