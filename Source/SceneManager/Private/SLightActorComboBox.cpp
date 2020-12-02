#include "SLightActorComboBox.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Input/SComboBox.h"    // SComboBox
#include "GameFramework/Actor.h"    // AActor
#include "Editor.h" // GEditor
#include "Editor/EditorEngine.h"    // UEditorEngine
#include "Engine/Level.h"   // ULevel
#include "Engine/Light.h"   // ALight
#include "Engine/World.h"   // UWorld
#include "Kismet/GameplayStatics.h" // UGameplayStatics

#define LOCTEXT_NAMESPACE "SLightActorComboBox"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightActorComboBox::Construct(const FArguments& InArgs)
{
    CurrentItem = MakeShareable(new FString("Default"));

    ChildSlot
    [
        SAssignNew(ComboBox, SComboBox<ItemType>)
        .OptionsSource(&Options)
        .OnSelectionChanged(this, &SLightActorComboBox::OnSelectionChanged)
        .OnGenerateWidget(this, &SLightActorComboBox::MakeWidgetForOption)
        .OnComboBoxOpening(this, &SLightActorComboBox::OnComboBoxOpening)
        .InitiallySelectedItem(CurrentItem)
        [
            SNew(STextBlock)
            .Text(this, &SLightActorComboBox::GetCurrentItemLabel)
        ]
    ];


 }
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText SLightActorComboBox::GetCurrentItemLabel() const
{
    if (CurrentItem.IsValid()) {
        return FText::FromString(*CurrentItem);
    }

    return LOCTEXT("InvalidComboEntryText", "<<Invalid option>>");
}

TSharedRef<SWidget> SLightActorComboBox::MakeWidgetForOption(ItemType InOption)
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SLightActorComboBox::OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type)
{
    if (Type == ESelectInfo::Type::Direct) {
        return;
    }
    CurrentItem = NewValue;
}

void SLightActorComboBox::OnComboBoxOpening()
{
    Options.Empty();

    UWorld* World = GEditor->GetEditorWorldContext().World();
    ULevel* Level = World->GetCurrentLevel();

    TArray<AActor *> ActorList;
    UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);
    UE_LOG(LogTemp, Warning, TEXT("ALight actor count: %d"), ActorList.Num());

    for (auto Actor : ActorList) {
        Options.Add(MakeShareable(new FString(Actor->GetName())));
    }

    ComboBox->RefreshOptions();
}

#undef LOCTEXT_NAMESPACE
