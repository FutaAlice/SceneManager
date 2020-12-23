#pragma once
#include <functional>
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"    // SComboBox

class APostProcessVolume;

/**
 * A combo box to select light actor in current opening map
 */
class SPostProcessComboBox : public SCompoundWidget
{
public:
    typedef TSharedPtr<FString> ItemType;
    SLATE_BEGIN_ARGS(SPostProcessComboBox)
    {
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
 
    FText GetCurrentItemLabel() const;
    bool SetByVolumeName(const FString& ActorName);
    
public:
    std::function<void(FString, APostProcessVolume *)> CB_SelectionChange;

private:
    TSharedRef<SWidget> OnGenerateWidget(ItemType InOption);
    void OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type);
    void OnComboBoxOpening();

    TSharedPtr<SComboBox<ItemType>> ComboBox;
    TArray<ItemType> Options;
    ItemType CurrentItem;
};
