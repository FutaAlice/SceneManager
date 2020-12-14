#include "SMaterialDetailsPanel.h"
#include "SlateOptMacros.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Widgets/Layout/SUniformGridPanel.h"   // SUniformGridPanel
#include "Materials/MaterialInstance.h" // UMaterialInstance
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Colors/SColorBlock.h" // SColorBlock
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Layout/SBox.h"    // SBox

#include "Engine.h"
#include "Engine/Engine.h"

#include "SceneManagementAssetData.h"

TSharedRef<SBoxPanel> SMaterialDetailsPanel::CreateVectorParamSlot(FString Key, FLinearColor Value, TSharedPtr<SColorBlock> ColorBlock)
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(STextBlock)
            .Text(FText::FromString(Key))
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()[SNew(SBox).WidthOverride(5)]
        + SHorizontalBox::Slot()
        .Padding(10, 0, 10, 0)
        [
            SAssignNew(ColorBlock, SColorBlock)
            .Color(this, &SMaterialDetailsPanel::GetVectorByName, Key)
            .ShowBackgroundForAlpha(true)
            .OnMouseButtonDown(this, &SMaterialDetailsPanel::OnClickColorBlock, Key)
        ];
}

FReply SMaterialDetailsPanel::OnClickColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FString Name)
{
    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnOk = false;
    PickerArgs.bOnlyRefreshOnMouseUp = true;

    PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
    PickerArgs.InitialColorOverride = GetVectorByName(Name);
    PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SMaterialDetailsPanel::OnColorCommitted, Name);
    PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP(this, &SMaterialDetailsPanel::OnColorPickerWindowClosed);
    PickerArgs.ParentWidget = nullptr;

    OpenColorPicker(PickerArgs);
    return FReply::Handled();
}

void SMaterialDetailsPanel::OnColorCommitted(FLinearColor NewColor, FString Name)
{
    MaterialInfo->VectorParams[Name] = NewColor;
    SyncToGridPanel();
}

void SMaterialDetailsPanel::OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window)
{
    // TODO
    if (MaterialInfo) {
        MaterialInfo->ToMaterial();
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialDetailsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> PlayerLightViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PlayerLightViewRef->OnFinishedChangingProperties().AddRaw(this, &SMaterialDetailsPanel::OnFinishedChangingProperties);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            PlayerLightViewRef
        ]
        + SVerticalBox::Slot()
        [
            SAssignNew(UniformGridPanel, SUniformGridPanel)
        ]
    ];

    DetailView = PlayerLightViewRef.operator->();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMaterialDetailsPanel::BindDataField(UObject* InObject)
{
    MaterialInfo = Cast<UMaterialInfo>(InObject);
    DetailView->SetObject(InObject);
    ResetDataBySelectedAsset();
}

UMaterialInfo* SMaterialDetailsPanel::GetDataField()
{
	return MaterialInfo;
}

void SMaterialDetailsPanel::ForceRefresh()
{
    DetailView->ForceRefresh();
}

void SMaterialDetailsPanel::ResetDataBySelectedAsset()
{
    UniformGridPanel->ClearChildren();

    MaterialInfo->FromMaterial();

    SyncToGridPanel();
}

void SMaterialDetailsPanel::SyncToGridPanel()
{
    UniformGridPanel->ClearChildren();
    int i = 0, j = 0, col = 2;
    for (auto Item : MaterialInfo->VectorParams) {
        TSharedPtr<SColorBlock> ColorBlock;
        UniformGridPanel->AddSlot(i, j)
            [
                CreateVectorParamSlot(Item.Key, Item.Value, ColorBlock)
            ];
        if (++i == col) {
            i = 0;
            j++;
        }
    }
}

void SMaterialDetailsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (InEvent.GetPropertyName() == "SoftObjectPath") {
        ResetDataBySelectedAsset();
    }
}

FLinearColor SMaterialDetailsPanel::GetVectorByName(FString Name) const 
{
    if (!MaterialInfo) {
        return FLinearColor();
    }
    return MaterialInfo->VectorParams[Name];
}
