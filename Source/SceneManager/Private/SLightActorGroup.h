#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SVerticalBox;
class SHorizontalBox;
class STextBlock;
class SLightItem;

/**
 * 
 */
class SLightActorGroup : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLightActorGroup)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void Clear();

	void OnAddLightItem();
	void OnSolutionChanged(int SolutionIndex);

public:
	TSharedPtr<SVerticalBox> MainLayout;
	TSharedPtr<STextBlock> TitleBlock;
	TSharedPtr<SHorizontalBox> ToolBarContainer;
	TSharedPtr<SVerticalBox> Groups;
	TArray<TSharedRef<SLightItem>> LightItemWidgets;
};
