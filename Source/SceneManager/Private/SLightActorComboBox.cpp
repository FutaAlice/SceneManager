#include "SLightActorComboBox.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"    // STextBlock
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
        .OnGenerateWidget(this, &SLightActorComboBox::OnGenerateWidget)
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

bool SLightActorComboBox::SetByActorName(const FString& ActorName)
{
    if (!CurrentItem.IsValid()) {
        return false;
    }

    UWorld* World = GEditor->GetEditorWorldContext().World();
    ULevel* Level = World->GetCurrentLevel();

    TArray<AActor*> ActorList;
    UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);

    for (auto Actor : ActorList) {
        if (Actor->GetName() == ActorName) {
            *CurrentItem = ActorName;
            return true;
        }
    }
    *CurrentItem = "";
    return false;
}

TSharedRef<SWidget> SLightActorComboBox::OnGenerateWidget(ItemType InOption)
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SLightActorComboBox::OnSelectionChanged(ItemType NewValue, ESelectInfo::Type Type)
{
    if (Type == ESelectInfo::Type::Direct) {
        return;
    }
    if (*CurrentItem != *NewValue) {
        CurrentItem = NewValue;

        UWorld* World = GEditor->GetEditorWorldContext().World();
        ULevel* Level = World->GetCurrentLevel();

        TArray<AActor *> ActorList;
        UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);
        for (auto Actor : ActorList) {
            if (Actor->GetName() == *NewValue) {
                ALight *Light = Cast<ALight>(Actor);
                if (CB_SelectionChange) {
                    CB_SelectionChange(*NewValue.Get(), Light);
                }
                return;
            }
        }

        ensure(false);
    }
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
        uint32 UniqueID = Actor->GetUniqueID();
        FString ObjectName = UKismetSystemLibrary::GetObjectName(Actor);
        FString ActorName = Actor->GetName();
        FString LevelName = Actor->GetLevel()->GetOuter()->GetName();

        Options.Add(MakeShareable(new FString(Actor->GetName())));
        // GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, ActorName + FString(" ") + ObjectName);
    }

    ComboBox->RefreshOptions();
}

#undef LOCTEXT_NAMESPACE
