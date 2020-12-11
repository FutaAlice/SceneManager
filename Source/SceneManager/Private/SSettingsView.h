#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "UObject/SoftObjectPath.h"

#include "SSettingsView.generated.h"

class UMaterialParameterCollection;
class USceneManagementAssetData;

UCLASS()
class UAssetWrap : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Select Scene Management Asset:")
        USceneManagementAssetData *AssetData;

    UPROPERTY(EditAnywhere, Category = "Select Lighting Asset:")
        UMaterialParameterCollection* SceneLightingMPC;

    UPROPERTY(EditAnywhere, Category = "Select Lighting Asset:")
        UMaterialParameterCollection* CharacterLightingMPC;

    UPROPERTY(EditAnywhere, Category = "Select Lighting Asset:")
        FStringAssetReference StringAssetReference;
};

/**
 *
 */
class SSettingsView : public SCompoundWidget
{
    friend USceneManagementAssetData;   // give accessibility to query current selected asset
public:
    SLATE_BEGIN_ARGS(SSettingsView)
    {
    }
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs);

    ~SSettingsView();

    static SSettingsView* Get();

    static UMaterialParameterCollection* GetSceneLightingMPC();
    static UMaterialParameterCollection* GetCharacterLightingMPC();
    
    void OnSceneManagementAssetChanged(const FPropertyChangedEvent& InEvent);

private:
    static SSettingsView *Instance;
    UAssetWrap *AssetWrap;
};
