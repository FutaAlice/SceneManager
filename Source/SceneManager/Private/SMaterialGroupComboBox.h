#pragma once
#include <functional>
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"    // SComboBox

/**
 * A combo box to select light actor in current opening map
 */
class SMaterialGroupComboBox : public SCompoundWidget
{
public:
    typedef TSharedPtr<FString> ItemType;
    SLATE_BEGIN_ARGS(SMaterialGroupComboBox)
    {
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
 
    FText GetCurrentItemLabel() const;
    void SetCurrentItemLabel(const FString& Label);
    
public:
    std::function<void(FString)> CB_SelectionChange;
    std::function<TArray<FString>()> FUNC_GetExistGroups;

private:
    TSharedRef<SWidget> OnGenerateWidget(ItemType InOption);
    void OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type);
    void OnComboBoxOpening();

    TSharedPtr<SComboBox<ItemType>> ComboBox;
    TArray<ItemType> Options;
    ItemType CurrentItem;
};
