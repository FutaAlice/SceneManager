// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ULightSettings;

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

private:
    TSharedPtr<ULightSettings> LightSettings;
};
