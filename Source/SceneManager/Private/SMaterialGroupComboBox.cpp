#include "SMaterialGroupComboBox.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"    // STextBlock

#define LOCTEXT_NAMESPACE "SMaterialGroupComboBox"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialGroupComboBox::Construct(const FArguments& InArgs)
{
    CurrentItem = MakeShareable(new FString("Default"));

    ChildSlot
        [
            SAssignNew(ComboBox, SComboBox<ItemType>)
            .OptionsSource(&Options)
            .OnSelectionChanged(this, &SMaterialGroupComboBox::OnSelectionChanged)
            .OnGenerateWidget(this, &SMaterialGroupComboBox::OnGenerateWidget)
            .OnComboBoxOpening(this, &SMaterialGroupComboBox::OnComboBoxOpening)
            .InitiallySelectedItem(CurrentItem)
            [
                SNew(STextBlock)
                .Text(this, &SMaterialGroupComboBox::GetCurrentItemLabel)
            ]
        ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText SMaterialGroupComboBox::GetCurrentItemLabel() const
{
    if (CurrentItem.IsValid()) {
        return FText::FromString(*CurrentItem);
    }

    return LOCTEXT("InvalidComboEntryText", "<<Invalid option>>");
}


TSharedRef<SWidget> SMaterialGroupComboBox::OnGenerateWidget(ItemType InOption)
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SMaterialGroupComboBox::OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type)
{
    if (Type == ESelectInfo::Type::Direct) {
        return;
    }
    if (*CurrentItem != *NewValue) {
        CurrentItem = NewValue;
        CB_SelectionChange(*NewValue.Get());
    }
}

void SMaterialGroupComboBox::OnComboBoxOpening()
{
    Options.Empty();

    ensure(FUNC_GetExistGroups);
    TArray<FString> OptionalGroupNames = FUNC_GetExistGroups();

    UE_LOG(LogTemp, Warning, TEXT("Optional Group Names Count: %d"), OptionalGroupNames.Num());

    for (auto GroupName : OptionalGroupNames) {
        Options.Add(MakeShareable(new FString(GroupName)));
        // GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, ActorName + FString(" ") + ObjectName);
    }

    ComboBox->RefreshOptions();
}

#undef LOCTEXT_NAMESPACE
