#include "SMaterialDetailsPanel.h"
#include "SlateOptMacros.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Widgets/Layout/SUniformGridPanel.h"   // SUniformGridPanel
#include "Materials/MaterialInstance.h" // UMaterialInstance
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Layout/SBox.h"    // SBox


#include "Engine.h"
#include "Engine/Engine.h"

#include "SceneManagementAssetData.h"

TSharedRef<SBoxPanel> SMaterialDetailsPanel::CreateVectorParamSlot(FString Key, FLinearColor Value, TSharedPtr<SImage> Image)
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(STextBlock)
            .Text(FText::FromString(Key))
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(5)
        ]
        + SHorizontalBox::Slot()
        .Padding(10, 0, 10, 0)
        [
            SAssignNew(Image, SImage)
            .ColorAndOpacity(FSlateColor(Value))
            .OnMouseButtonDown(this, &SMaterialDetailsPanel::OnClickColorBlock, Key, Value)
        ];
}

FReply SMaterialDetailsPanel::OnClickColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FString Key, FLinearColor Value)
{
    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnOk = false;
    PickerArgs.bOnlyRefreshOnMouseUp = true;

    PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
    PickerArgs.InitialColorOverride = Value;
    //PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SMaterialDetailsPanel::OnColorCommitted, Key, Value);
    //PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP(this, &SMaterialDetailsPanel::OnColorPickerWindowClosed);
    PickerArgs.ParentWidget = nullptr;

    OpenColorPicker(PickerArgs);
    return FReply::Handled();
}

//void SMaterialDetailsPanel::OnColorCommitted(FString Name, FLinearColor NewColor)
//{
//    MaterialInfo->VectorParams[Name] = NewColor;
//    SyncToGridPanel();
//}

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
    FSoftObjectPath SoftObjectPath = MaterialInfo->SoftObjectPath;
    if (SoftObjectPath.IsNull()) {
        // TODO: clear
        UniformGridPanel->ClearChildren();
        MaterialInfo->VectorParams.Empty();
        MaterialInfo->ScalarParams.Empty();
        return;
    }

    UObject* Object = SoftObjectPath.ResolveObject();
    if (!Object) {
        Object = SoftObjectPath.TryLoad();
    }

    UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(Object);
    ensure(MaterialInstance);

    TArray<FMaterialParameterInfo> ParameterInfos;
    TArray<FGuid> ParameterGuids;

    MaterialInstance->GetAllVectorParameterInfo(ParameterInfos, ParameterGuids);

    for (auto MaterialParameterInfo : ParameterInfos) {
        FString Key = MaterialParameterInfo.Name.ToString();
        FLinearColor Value;
        MaterialInstance->GetVectorParameterValue(MaterialParameterInfo, Value);
        MaterialInfo->VectorParams.Add(Key, Value);
    }

    SyncToGridPanel();
}

void SMaterialDetailsPanel::SyncToGridPanel()
{
    UniformGridPanel->ClearChildren();
    int i = 0, j = 0, col = 2;
    for (auto Item : MaterialInfo->VectorParams) {
        TSharedPtr<SImage> Image;
        UniformGridPanel->AddSlot(i, j)
            [
                CreateVectorParamSlot(Item.Key, Item.Value, Image)
            ];
        if (++i == col) {
            i = 0;
            j++;
        }
    }
}

void SMaterialDetailsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    ResetDataBySelectedAsset();
}
