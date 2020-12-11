#include "SceneManagementAssetFactory.h"
#include "SceneManagementAssetData.h"

USceneManagementAssetFactory::USceneManagementAssetFactory()
    :Super()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = USceneManagementAsset::StaticClass();
}

UObject* USceneManagementAssetFactory::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName,
    EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn, FName CallingContext)
{
    auto NewObjectAsset = NewObject<USceneManagementAsset>(InParent, InClass, InName, Flags);
    return NewObjectAsset;
}
