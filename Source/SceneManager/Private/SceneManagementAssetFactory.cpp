#include "SceneManagementAssetFactory.h"
#include "SceneManagementAssetData.h"

USceneManagementAssetFactory::USceneManagementAssetFactory()
    :Super()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = USceneManagementAssetData::StaticClass();
}

UObject* USceneManagementAssetFactory::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName,
    EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn, FName CallingContext)
{
    auto NewObjectAsset = NewObject<USceneManagementAssetData>(InParent, InClass, InName, Flags);
    return NewObjectAsset;
}
