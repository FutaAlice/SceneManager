#pragma once
#include <Functional>
#include "CoreMinimal.h"
#include "Styling/SlateTypes.h" // ECheckBoxState

class SCheckBox;
class SVerticalBox;
class SHorizontalBox;

class SolutionSelector
{
public:
    SolutionSelector();

    TSharedRef<SWidget> Self();
    void AddSolution(FString SolutionName, FString SolutionToolTip);

private:
    void Initialize();

    /**
     * @brief Update solution tabs check state.
     * 
     * @param CheckedIndex The checked index of solution widgets, -1 for not select.
     */
    void UpdateClickButtonState(int CheckedIndex = -1);

    /**
     * @brief Infer the SCheckBox index user clicked from previous click index and current check state.
     * 
     * @param CheckState the new SCheckBox state after user click.
     * @return int 
     */
    int InferClickedButtonIndex(ECheckBoxState CheckState);

private:
    TSharedRef<SVerticalBox> MainLayout;
    TSharedRef<SVerticalBox> SolutionWidgetContainer;
    TSharedRef<SHorizontalBox> ToolBarContainer;

    TArray<TSharedRef<SCheckBox>> SlateWidgetRef;

    int CurrentSelectedSolutionIndex;

public:
    std::function<void(int)> CB_Append;
    std::function<void(int)> CB_Remove;
    std::function<void(int, FText)> CB_Rename;
    std::function<void(int)> CB_Active;
};
