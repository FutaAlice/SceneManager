#include "SMaterialDetailsPanel.h"
#include "SlateOptMacros.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "IDetailCustomization.h"   // IDetailCustomization
#include "DetailLayoutBuilder.h"    // IDetailLayoutBuilder
#include "DetailCategoryBuilder.h"  // IDetailCategoryBuilder
#include "DetailWidgetRow.h"    // FDetailWidgetRow
#include "Materials/MaterialInstance.h" // UMaterialInstance
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Colors/SColorBlock.h" // SColorBlock
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Layout/SBox.h"    // SBox

#include "Editor.h"
#include "Engine.h"
#include "Editor/EditorEngine.h"
#include "Engine/Engine.h"

#include "SceneManagementAssetData.h"

class FMaterialInfoCustomization : public IDetailCustomization
{
public:
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShareable(new FMaterialInfoCustomization);
    }

    float GetScalar(FString Name) const;
    void SetScalar(float NewValue, FString Name);

    FLinearColor GetVector(FString Name) const;
    void SetVector(FLinearColor NewValue, FString Name);

    FReply OnClickColorBlock(const FGeometry&, const FPointerEvent&, FString Name);

    UMaterialInfo* MaterialInfo = nullptr;
};

float FMaterialInfoCustomization::GetScalar(FString Name) const
{
    if (!MaterialInfo) {
        return 0.0f;
    }
    float *Value = MaterialInfo->ScalarParams.Find(Name);
    if (!Value) {
        return 0.0f;
    }
    return *Value;
}

void FMaterialInfoCustomization::SetScalar(float NewValue, FString Name)
{
    if (!MaterialInfo) {
        return;
    }
    if (float *Value = MaterialInfo->ScalarParams.Find(Name)) {
        *Value = NewValue;
    }
}

FLinearColor FMaterialInfoCustomization::GetVector(FString Name) const
{
    if (!MaterialInfo) {
        return FLinearColor();
    }
    FLinearColor *Value = MaterialInfo->VectorParams.Find(Name);
    if (!Value) {
        return FLinearColor();
    }
    return *Value;
}

void FMaterialInfoCustomization::SetVector(FLinearColor NewValue, FString Name)
{
    if (!MaterialInfo) {
        return;
    }
    if (FLinearColor *Value = MaterialInfo->VectorParams.Find(Name)) {
        *Value = NewValue;
    }
}

FReply FMaterialInfoCustomization::OnClickColorBlock(const FGeometry&, const FPointerEvent&, FString Name)
{
    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnOk = false;
    PickerArgs.bOnlyRefreshOnMouseUp = false;
    PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
    PickerArgs.InitialColorOverride = GetVector(Name);
    PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FMaterialInfoCustomization::SetVector, Name);
    PickerArgs.ParentWidget = nullptr;
    OpenColorPicker(PickerArgs);
    return FReply::Handled();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FMaterialInfoCustomization::CustomizeDetails(IDetailLayoutBuilder & DetailBuilder)
{
    const IDetailsView* DetailView = DetailBuilder.GetDetailsView();
    const TArray<TWeakObjectPtr<UObject>>& SelectedObjects = DetailView->GetSelectedObjects();

    for (int32 ObjectIndex = 0; !MaterialInfo && ObjectIndex < SelectedObjects.Num(); ++ObjectIndex) {
        const TWeakObjectPtr<UObject>& CurrentObject = SelectedObjects[ObjectIndex];
        if (CurrentObject.IsValid()) {
            MaterialInfo = Cast<UMaterialInfo>(CurrentObject.Get());
        }
    }

    TArray<FName> CategoryNames;
    DetailBuilder.GetCategoryNames(CategoryNames);
    ensure(CategoryNames.Num() >= 1);
    
    TSharedPtr<SVerticalBox> VerticalBox;
    DetailBuilder.EditCategory(CategoryNames[0], FText::GetEmpty(), ECategoryPriority::Default)
        .AddCustomRow(FText::GetEmpty(), true)[SAssignNew(VerticalBox, SVerticalBox)];

    if (!MaterialInfo) {
        return;
    }

    for (auto Item : MaterialInfo->ScalarParams) {
        const FString ParameterName = Item.Key;
        TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromString(ParameterName))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSpinBox<float>)
                .Value(this, &FMaterialInfoCustomization::GetScalar, ParameterName)
                .OnValueChanged_Raw(this, &FMaterialInfoCustomization::SetScalar, ParameterName)
                .MinValue(TOptional<float>())
                .MaxValue(TOptional<float>())
            ];
        VerticalBox->AddSlot().Padding(1, 1, 1, 1)[HorizontalBox];
    }

    for (auto Item : MaterialInfo->VectorParams) {
        const FString ParameterName = Item.Key;
        TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromString(ParameterName))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SColorBlock)
                .Color(this, &FMaterialInfoCustomization::GetVector, ParameterName)
                .ShowBackgroundForAlpha(true)
                .OnMouseButtonDown(this, &FMaterialInfoCustomization::OnClickColorBlock, ParameterName)
            ];
        VerticalBox->AddSlot().Padding(1, 1, 1, 1)[HorizontalBox];
    }
}

void SMaterialDetailsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMaterialDetailsPanel::OnFinishedChangingProperties);
    DetailsViewRef->RegisterInstancedCustomPropertyLayout(UMaterialInfo::StaticClass(),
        FOnGetDetailCustomizationInstance::CreateLambda([] { return FMaterialInfoCustomization::MakeInstance(); }));

    ChildSlot
    [
        DetailsViewRef
    ];

    DetailView = DetailsViewRef.operator->();
    MaterialInfo = nullptr;
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
    MaterialInfo->FromMaterial();
    DetailView->ForceRefresh();
    //SyncToGridPanel();
}

//void SMaterialDetailsPanel::SyncToGridPanel()
//{
//    UniformGridPanel->ClearChildren();
//    int i = 0, j = 0, col = 2;
//    for (auto Item : MaterialInfo->VectorParams) {
//        TSharedPtr<SColorBlock> ColorBlock;
//        UniformGridPanel->AddSlot(i, j)
//            [
//                CreateVectorParamSlot(Item.Key, Item.Value, ColorBlock)
//            ];
//        if (++i == col) {
//            i = 0;
//            j++;
//        }
//    }
//}

void SMaterialDetailsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (InEvent.GetPropertyName() == "SoftObjectPath") {
        ResetDataBySelectedAsset();
    }
}
