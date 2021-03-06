#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class AActor;
class ULightParams;
class IDetailsView;

/**
 * 
 */
class SLightActorDetailPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLightActorDetailPanel)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

    // void SetParam(ULightParams *InData);
    ULightParams * GetParam();

    void SetActor(AActor *InActor);
    void BindDataField(UObject* InObject);
    ULightParams* GetDataField();
    void ForceRefresh();

private:
    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

private:
    IDetailsView* PlayerLightView;
    ULightParams *LightParams;
};
