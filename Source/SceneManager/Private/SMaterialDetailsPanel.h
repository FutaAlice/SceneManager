// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UMaterialInstance;
class UMaterialInfo;
class IDetailsView;

/**
 * 
 */
class SMaterialDetailsPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMaterialDetailsPanel)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

    void BindDataField(UObject* InObject);
    UMaterialInfo* GetDataField();
    void ForceRefresh();

private:
    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

private:
    IDetailsView* DetailView;
    UMaterialInfo* MaterialInfo;
};
