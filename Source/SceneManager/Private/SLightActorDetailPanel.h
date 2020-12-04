#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class AActor;
class ULightParams;

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

    ~SLightActorDetailPanel();

    // void SetParam(ULightParams *InData);
    ULightParams * GetParam();

    void BindActor(AActor *InActor);

private:
    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

private:
    AActor *Light;
    ULightParams *LightParams;
};
