#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "SSettingsView.generated.h"

UCLASS()
class UXXX : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Asset")
        int fuck;
    UPROPERTY(EditAnywhere, Category = "Asset")
        class USceneManagementAsset *MyAsset;
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

    UXXX *SelectedUXXX;

    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);
};
