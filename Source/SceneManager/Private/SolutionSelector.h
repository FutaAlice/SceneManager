#pragma once
#include <Functional>
#include "CoreMinimal.h"
#include "Styling/SlateTypes.h" // ECheckBoxState

class SCheckBox;
class STextBlock;
class SVerticalBox;
class SHorizontalBox;

class FSolutionSelector
{
public:
    FSolutionSelector();

    TSharedRef<SWidget> Self();

    /**
     * @brief Add a new solution with default name
     * 
     */
    void AddSolution(bool bCallback = true);

    /**
     * @brief Add a new solution with specified name and tooltip
     * 
     * @param SolutionName 
     * @param SolutionToolTip 
     */
    void AddSolution(FString SolutionName, FString SolutionToolTip, bool bCallback = true);

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
     * @brief Removes an solution at given location
     * 
     * @param SolutionIndex Location in array of the solution to remove.
     */
    void RemoveSolution(int SolutionIndex);

    void DuplicateSolution(int SolutionIndex);

    int GetCurrentSelectedSolutionIndex();

    /**
     * @brief Returns number of solutions in current selector
     * 
     * @return int 
     */
    int Num();

    /**
     * @brief 
     * 
     */
    void Clear();

    /**
     * @brief Update solution tabs check state
     * 
     * @param CheckedIndex The checked index of solution widgets, -1 for not select
     */
    void UpdateClickButtonState(int CheckedIndex = -1);

private:
    /**
     * @brief Update all solution tooltip, automatically called after remove solution
     */
    void UpdateToolTips();

    /**
     * @brief Infer the SCheckBox index user clicked from previous click index and current check state
     * 
     * @param CheckState the new SCheckBox state after user click
     * @return int 
     */
    int InferClickedButtonIndex(ECheckBoxState CheckState);

    void CreateRenameDialog(int Index);

private:
    TSharedPtr<SVerticalBox> MainLayout;
    TSharedPtr<SVerticalBox> SolutionWidgetContainer;
    TSharedPtr<SHorizontalBox> ToolBarContainer;

    TArray<TSharedRef<SCheckBox>> SlateWidgetRef;
    TMap<TSharedRef<SCheckBox>, TSharedRef<STextBlock>> SolutionTextMapping;

    int CurrentSelectedSolutionIndex = -1;

public:
    std::function<void(int)> CB_Active;
    std::function<void(int)> CB_Append;
    std::function<void(int)> CB_Duplicate;
    std::function<void(int)> CB_Remove;
    std::function<void(int, FString)> CB_Rename;
};
