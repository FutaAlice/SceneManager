#pragma once
#include "CoreMinimal.h"

class SWidget;

class SolutionSelector
{
public:
    SolutionSelector();

    TSharedRef<SWidget> Self();
    void AddSolution(FString SolutionName, FString SolutionToolTip);

private:
    void AppendButtons();


private:
    TSharedRef<class SVerticalBox> SolutionTabContainer;
};
