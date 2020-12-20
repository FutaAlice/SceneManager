#pragma once
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class IDetailCustomization;
class UMaterialParameterCollection;

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

	void SetObject(UMaterialParameterCollection* InObject);

private:
	TSharedRef<IDetailCustomization> CreateDetailCustomizationInstance();
	void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

private:
	IDetailsView* DetailsView;
	UMaterialParameterCollection* MPC;  // unused
};
