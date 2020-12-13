#pragma once
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;

/**
 * 
 */
class SMPCDetialsPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMPCDetialsPanel)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetObject(class UMaterialParameterCollection* InObject);

private:
	void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

private:
	IDetailsView* DetailsView;

};
