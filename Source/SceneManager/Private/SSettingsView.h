#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "UObject/SoftObjectPath.h"

#include "SSettingsView.generated.h"

class UMaterialParameterCollection;
class USceneManagementAssetData;
class IDetailsView;

UCLASS()
class UAssetWrap : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Select Scene Management Asset")
        USceneManagementAssetData *AssetData;

    UPROPERTY(EditAnywhere, Category = "MPC")
        UMaterialParameterCollection* SceneLightingMPC;

    UPROPERTY(EditAnywhere, Category = "MPC")
        UMaterialParameterCollection* CharacterLightingMPC;

    UPROPERTY(EditAnywhere, Category = "MPC")
        UMaterialParameterCollection* WindMPC;

    UPROPERTY(EditAnywhere, Category = "MPC")
        UMaterialParameterCollection* PostProcessingMPC;
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
    
private:
    void OnSceneManagementAssetChanged(const FPropertyChangedEvent& InEvent);
    FReply SaveAssetData();

private:
    static SSettingsView *Instance;
    UAssetWrap *AssetWrap;

    IDetailsView* GroupNameView;
};
