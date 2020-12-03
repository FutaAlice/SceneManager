#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ALight;
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

    bool SetParam(ULightParams *InData);
    TSharedPtr<ULightParams> GetParam() const;

    bool BindActor(ALight *InActor);

private:

    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);
    void OnLightDestroyed(AActor *);


private:
    ALight *Light;
    TSharedPtr<ULightParams> LightParams;
};
