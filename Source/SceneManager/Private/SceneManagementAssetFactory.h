#pragma once
#include "Factories/Factory.h"
#include "SceneManagementAssetFactory.generated.h"

/**
 * Custom Asset Factory
 */
UCLASS()
class USceneManagementAssetFactory : public UFactory
{
    GENERATED_BODY()
public:
    USceneManagementAssetFactory();

    virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
        UObject *Context, FFeedbackContext *Warn, FName CallingContext) override;
};
