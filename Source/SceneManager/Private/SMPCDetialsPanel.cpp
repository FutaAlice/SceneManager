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



//void FMPCDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
//{
//    auto DetailView = DetailBuilder.GetDetailsView();
//    const TArray< TWeakObjectPtr<UObject>>& SelectedObjects = DetailView->GetSelectedObjects();
//
//    for (int32 ObjectIndex = 0; !MPC.IsValid() && ObjectIndex < SelectedObjects.Num(); ++ObjectIndex) {
//        const TWeakObjectPtr<UObject>& CurrentObject = SelectedObjects[ObjectIndex];
//        if (CurrentObject.IsValid()) {
//            MPC = Cast<UMaterialParameterCollection>(CurrentObject.Get());
//        }
//    }
//
//    DetailBuilder.EditCategory("CustomCategory", FText::GetEmpty(), ECategoryPriority::Important)
//        .AddCustomRow(FText::GetEmpty())
//        [
//            SNew(SVerticalBox)
//            + SVerticalBox::Slot()
//            .VAlign(VAlign_Center)
//            [
//                SNew(SColorPicker)
//                .OnColorCommitted(this, &FMPCDetailsCustomization::ColorPicked)
//            ]
//        ];
//}
//
//void FMPCDetailsCustomization::ColorPicked(FLinearColor SelectedColor)
//{
//    if (MPC.IsValid()) {
//        SelectedColor.ToFColor(false).ToHex();
//    }
//}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMPCDetialsPanel::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
	TSharedRef<IDetailsView> DetailsViewRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsViewRef->OnFinishedChangingProperties().AddRaw(this, &SMPCDetialsPanel::OnFinishedChangingProperties);
    //DetailsViewRef->RegisterInstancedCustomPropertyLayout(UMaterialParameterCollection::StaticClass(),
    //    FOnGetDetailCustomizationInstance::CreateRaw(this, &SMPCDetialsPanel::CreateDetailCustomizationInstance));

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

//TSharedRef<IDetailCustomization> SMPCDetialsPanel::CreateDetailCustomizationInstance()
//{
//    return FMPCDetailsCustomization::MakeInstance();
//}

void SMPCDetialsPanel::OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent)
{
}
