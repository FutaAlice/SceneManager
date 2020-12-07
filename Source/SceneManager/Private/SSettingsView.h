#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "SSettingsView.generated.h"

class USceneManagementAsset;

UCLASS()
class UAssetWrap : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Select Scene Management Asset:")
    USceneManagementAsset *SceneManagementAsset;
};

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

    void OnSceneManagementAssetChanged(const FPropertyChangedEvent& InEvent);

private:
    static SSettingsView *Instance;
    UAssetWrap *AssetWrap;
};
