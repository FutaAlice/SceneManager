#include "SMPCDetialsPanel.h"

#include "SlateOptMacros.h"
#include "Modules/ModuleManager.h"  // FModuleManager
#include "IDetailsView.h"   // IDetailsView, FDetailsViewArgs
#include "PropertyEditorModule.h"   // FPropertyEditorModule
#include "Materials/MaterialParameterCollection.h"	// UMaterialParameterCollection
#include "IDetailCustomization.h"   // IDetailCustomization
#include "DetailLayoutBuilder.h"    // IDetailLayoutBuilder
#include "DetailCategoryBuilder.h"  // IDetailCategoryBuilder
#include "DetailWidgetRow.h"    // FDetailWidgetRow
#include "Widgets/SBoxPanel.h"  
#include "Widgets/Colors/SColorPicker.h"    // SColorPicker
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Input/SSpinBox.h" // SSpinBox
#include "Widgets/Colors/SColorBlock.h" // SColorBlock
#include "Engine/Engine.h"
#include "Engine.h"

class FMPCDetailsCustomization : public IDetailCustomization
{
public:
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShareable(new FMPCDetailsCustomization);
    }

    float GetScalar(FName Name) const;
    void SetScalar(float NewValue, FName Name);

    FLinearColor GetVector(FName Name) const;
    void SetVector(FLinearColor NewValue, FName Name);

    FReply OnClickColorBlock(const FGeometry&, const FPointerEvent&, FName Name);

    TWeakObjectPtr<UMaterialParameterCollection> MPC;
};

float FMPCDetailsCustomization::GetScalar(FName Name) const
{
    return MPC->GetScalarParameterByName(Name)->DefaultValue;
}

void FMPCDetailsCustomization::SetScalar(float NewValue, FName Name)
{
    FCollectionScalarParameter* ScalarParameter = const_cast<FCollectionScalarParameter*>(MPC->GetScalarParameterByName(Name));
    ScalarParameter->DefaultValue = NewValue;
}

FLinearColor FMPCDetailsCustomization::GetVector(FName Name) const
{
    return MPC->GetVectorParameterByName(Name)->DefaultValue;
}

void FMPCDetailsCustomization::SetVector(FLinearColor NewValue, FName Name)
{
    FCollectionVectorParameter* ScalarParameter = const_cast<FCollectionVectorParameter*>(MPC->GetVectorParameterByName(Name));
    ScalarParameter->DefaultValue = NewValue;
}

FReply FMPCDetailsCustomization::OnClickColorBlock(const FGeometry&, const FPointerEvent&, FName Name)
{
    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnOk = false;
    PickerArgs.bOnlyRefreshOnMouseUp = false;
    PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
    PickerArgs.InitialColorOverride = GetVector(Name);
    PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FMPCDetailsCustomization::SetVector, Name);
    PickerArgs.ParentWidget = nullptr;
    OpenColorPicker(PickerArgs);
    return FReply::Handled();
}

void FMPCDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    auto DetailView = DetailBuilder.GetDetailsView();
    const TArray< TWeakObjectPtr<UObject>>& SelectedObjects = DetailView->GetSelectedObjects();

    for (int32 ObjectIndex = 0; !MPC.IsValid() && ObjectIndex < SelectedObjects.Num(); ++ObjectIndex) {
        const TWeakObjectPtr<UObject>& CurrentObject = SelectedObjects[ObjectIndex];
        if (CurrentObject.IsValid()) {
            MPC = Cast<UMaterialParameterCollection>(CurrentObject.Get());
        }
    }

    TSharedPtr<SVerticalBox> VerticalBox;
    DetailBuilder.HideCategory("Material");
    DetailBuilder.EditCategory("MPC", FText::GetEmpty(), ECategoryPriority::Important)
        .AddCustomRow(FText::GetEmpty())[SAssignNew(VerticalBox, SVerticalBox)];

    for (FCollectionScalarParameter &ScalarParameter : MPC->ScalarParameters) {
        const FName ParameterName = ScalarParameter.ParameterName;
        TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromString(ParameterName.ToString()))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSpinBox<float>)
                .Value(this, &FMPCDetailsCustomization::GetScalar, ParameterName)
                .OnValueChanged_Raw(this, &FMPCDetailsCustomization::SetScalar, ParameterName)
                .MinValue(TOptional<float>())
                .MaxValue(TOptional<float>())

            ];
            VerticalBox->AddSlot().Padding(1, 1, 1, 1)[HorizontalBox];
    }

    for (FCollectionVectorParameter &VectorParameter : MPC->VectorParameters) {
        const FName ParameterName = VectorParameter.ParameterName;
        TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromString(ParameterName.ToString()))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SColorBlock)
                .Color(this, &FMPCDetailsCustomization::GetVector, ParameterName)
                .ShowBackgroundForAlpha(true)
                .OnMouseButtonDown(this, &FMPCDetailsCustomization::OnClickColorBlock, ParameterName)
            ];
        VerticalBox->AddSlot().Padding(1, 1, 1, 1)[HorizontalBox];
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMPCDetialsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMPCDetialsPanel::OnFinishedChangingProperties);
    DetailsViewRef->RegisterInstancedCustomPropertyLayout(UMaterialParameterCollection::StaticClass(),
        FOnGetDetailCustomizationInstance::CreateRaw(this, &SMPCDetialsPanel::CreateDetailCustomizationInstance));

    ChildSlot
    [
        DetailsViewRef
    ];

    DetailsView = DetailsViewRef.operator->();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMPCDetialsPanel::SetObject(UMaterialParameterCollection* InObject)
{
    MPC = InObject;
    DetailsView->SetObject(InObject);
}

TSharedRef<IDetailCustomization> SMPCDetialsPanel::CreateDetailCustomizationInstance()
{
    return FMPCDetailsCustomization::MakeInstance();
}

void SMPCDetialsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{

}
