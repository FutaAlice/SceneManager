#pragma once
#include <functional>
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ALight;

/**
 * A combo box to select light actor in current opening map
 */
class SLightActorComboBox : public SCompoundWidget
{
public:
    typedef TSharedPtr<FString> ItemType;
    SLATE_BEGIN_ARGS(SLightActorComboBox)
    {
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
 
    FText GetCurrentItemLabel() const;
    
public:
    std::function<void(FString, ALight *)> CB_SelectionChange;

private:
    TSharedRef<SWidget> OnGenerateWidget(ItemType InOption);
    void OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type);
    void OnComboBoxOpening();

    TSharedPtr<SComboBox<ItemType>> ComboBox;
    TArray<ItemType> Options;
    ItemType CurrentItem;
};
