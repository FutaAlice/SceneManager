#pragma once
#include <Functional>
#include "CoreMinimal.h"
#include "Styling/SlateTypes.h" // ECheckBoxState

class SCheckBox;
class STextBlock;
class SVerticalBox;
class SHorizontalBox;

class SolutionSelector
{
public:
    SolutionSelector();

    TSharedRef<SWidget> Self();

    void AddSolution();
    void AddSolution(FString SolutionName, FString SolutionToolTip);

    void RemoveSolution(int SolutionIndex);

private:
    void Initialize();

    /**
     * @brief Update solution tabs check state
     * 
     * @param CheckedIndex The checked index of solution widgets, -1 for not select
     */
    void UpdateClickButtonState(int CheckedIndex = -1);

    /**
     * @brief Update all solution tooltip, automatically called after remove solution
     */
    void UpdateToolTips();

    /**
     * @brief Reset solution name and tooltip
     * 
     * @param SolutionIndex Target solution widget index
     * @param SolutionName Name
     * @param SolutionToolTip ToolTip
     * @param Callback Should call user callback function or not
     */
    void RenameSolution(int SolutionIndex, FString SolutionName, FString SolutionToolTip, bool Callback = true);

    /**
     * @brief Infer the SCheckBox index user clicked from previous click index and current check state
     * 
     * @param CheckState the new SCheckBox state after user click
     * @return int 
     */
    int InferClickedButtonIndex(ECheckBoxState CheckState);

private:
    TSharedRef<SVerticalBox> MainLayout;
    TSharedRef<SVerticalBox> SolutionWidgetContainer;
    TSharedRef<SHorizontalBox> ToolBarContainer;

    TArray<TSharedRef<SCheckBox>> SlateWidgetRef;
    TMap<TSharedRef<SCheckBox>, TSharedRef<STextBlock>> SolutionTextMapping;

    int CurrentSelectedSolutionIndex;

public:
    std::function<void(int)> CB_Append;
    std::function<void(int)> CB_Remove;
    std::function<void(int, FString)> CB_Rename;
    std::function<void(int)> CB_Active;
};
