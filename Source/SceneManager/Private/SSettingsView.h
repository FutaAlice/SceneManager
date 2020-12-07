#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "SSettingsView.generated.h"

class UMaterialParameterCollection;
class USceneManagementAsset;

UCLASS()
class UAssetWrap : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Select Scene Management Asset:")
        USceneManagementAsset *SceneManagementAsset;

    UPROPERTY(EditAnywhere, Category = "Select Lighting Asset:")
        UMaterialParameterCollection* SceneLightingMPC;

    UPROPERTY(EditAnywhere, Category = "Select Lighting Asset:")
        UMaterialParameterCollection* CharacterLightingMPC;
};

//UCLASS()
//class UAssetWrap : public UObject
//{
//    GENERATED_BODY()
//public:
//    UPROPERTY(EditAnywhere, Category = "Select Scene Management Asset:")
//        USceneManagementAsset* SceneManagementAsset;
//};

/**
 *
 */
class SSettingsView : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSettingsView)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    static USceneManagementAsset* GetSceneManagementAsset(bool bShowMsgDialog = true);
    static USceneManagementAsset* GetSceneManagementNullAsset();

    static UMaterialParameterCollection* GetSceneLightingMPC();

    void OnSceneManagementAssetChanged(const FPropertyChangedEvent& InEvent);

private:
    static SSettingsView *Instance;
    UAssetWrap *AssetWrap;
};
