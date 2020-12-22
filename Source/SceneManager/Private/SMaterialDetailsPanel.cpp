#include "SMaterialDetailsPanel.h"
#include "SlateOptMacros.h"
#include "Misc/MessageDialog.h" // FMessageDialog
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
#include "Materials/MaterialInstance.h" // UMaterialInstance

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
        if (*Value != NewValue) {
            *Value = NewValue;
            MaterialInfo->ToMaterial();
        }
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
        if (*Value != NewValue) {
            *Value = NewValue;
            MaterialInfo->ToMaterial();
        }
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

static void DEBUG_PRINT_GROUP_NAME(UMaterialInstance* MaterialInstance)
{
    //TArray<FGuid> OutParameterIds;
    //TArray<FMaterialParameterInfo> OutParameterInfo;
    //MaterialInstance->GetAllParameterInfo<UMaterialExpressionParameter>(OutParameterInfo, OutParameterIds);
    //for (auto MaterialParaterInfo : OutParameterInfo) {
    //    FName OutGroup;
    //    MaterialInstance->GetGroupName(MaterialParaterInfo, OutGroup);
    //    UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *OutGroup.ToString(), *MaterialParaterInfo.Name.ToString());
    //}
}


static FName GetMaterialGroupName(UMaterialInstance* MaterialInstance, FName ParameterName)
{
    TArray<FGuid> OutParameterIds;
    TArray<FMaterialParameterInfo> OutParameterInfo;
    MaterialInstance->GetAllParameterInfo<UMaterialExpressionParameter>(OutParameterInfo, OutParameterIds);
    for (auto MaterialParaterInfo : OutParameterInfo) {
        if (MaterialParaterInfo.Name == ParameterName) {
            FName OutGroup;
            MaterialInstance->GetGroupName(MaterialParaterInfo, OutGroup);
            return OutGroup;
        }
    }
    return FName();
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

    UMaterialInstance* Material = MaterialInfo->GetMaterial<UMaterialInstance>();
    if (!Material) {
        return;
    }
    DEBUG_PRINT_GROUP_NAME(Material);

    for (auto Item : MaterialInfo->ScalarParams) {
        const FString ParameterName = Item.Key;
        const FString ParameterGroup = GetMaterialGroupName(Material, FName(*ParameterName)).ToString();
        if (!UEnabledGroupName::Contains(ParameterGroup)) {
            continue;
        }
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
        const FString ParameterGroup = GetMaterialGroupName(Material, FName(*ParameterName)).ToString();
        if (!UEnabledGroupName::Contains(ParameterGroup)) {
            continue;
        }
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
    ForceRefresh();
}

UMaterialInfo* SMaterialDetailsPanel::GetDataField()
{
	return MaterialInfo;
}

void SMaterialDetailsPanel::ForceRefresh()
{
    DetailView->ForceRefresh();
}

void SMaterialDetailsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
    if (InEvent.GetPropertyName() == "SoftObjectPath") {
        FSoftObjectPath SoftObjectPath = MaterialInfo->SoftObjectPath;
        MaterialInfo->SoftObjectPath.Reset();

        USceneManagementAssetData* AssetData = USceneManagementAssetData::GetSelected();
        ensure(AssetData);

        if (SoftObjectPath.IsNull()) {
            // TODO
        }
        else {
            int Err = AssetData->ReplaceMaterial(MaterialInfo, SoftObjectPath);
            if (Err == ERR_MAT_EXIST_INS) {
                FString Message = "SoftObjectPath: ";
                Message += SoftObjectPath.ToString();
                Message += ", already exist.";
                FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message));
            }
        }

        ForceRefresh();
    } 
    else {
        UE_LOG(LogTemp, Warning, TEXT("SMaterialDetailsPanel::OnFinishedChangingProperties"));
    }
}
