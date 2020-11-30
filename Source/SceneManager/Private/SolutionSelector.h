#pragma once
#include <Functional>
#include "CoreMinimal.h"

class SCheckBox;
class SVerticalBox;

class SolutionSelector
{
public:
    SolutionSelector();

    TSharedRef<SWidget> Self();
    void AddSolution(FString SolutionName, FString SolutionToolTip);

private:
    void AppendButtons();

private:
    TSharedRef<SVerticalBox> SolutionTabContainer;
    TArray<TSharedRef<SCheckBox>> Widgets;

public:
    std::function<void(int)> CB_Append;
    std::function<void(int)> CB_Remove;
    std::function<void(int)> CB_Rename;
    std::function<void(int)> CB_Active;
};
