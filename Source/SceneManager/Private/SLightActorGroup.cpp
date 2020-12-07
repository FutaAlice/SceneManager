#include "SLightActorGroup.h"
#include "SlateOptMacros.h"
#include "EditorStyleSet.h" // FEditorStyle
#include "Widgets/SBoxPanel.h"  // SVerticalBox, SHorizontalBox
#include "Widgets/SWindow.h"    // SWindow
#include "Widgets/Input/SButton.h"  // SButton
#include "Widgets/Input/SCheckBox.h"    // SCheckBox
#include "Widgets/Input/SEditableText.h"    // SEditableText
#include "Widgets/Images/SImage.h"  // SImage
#include "Widgets/Text/STextBlock.h"    // STextBlock
#include "Widgets/Input/SComboBox.h"	// FArguments
#include "Widgets/Layout/SSpacer.h"	// SSpacer

#include "InternalDataStructure.h"
#include "SceneManagementAsset.h"
#include "SolutionSelector.h"
#include "SLightActorComboBox.h"
#include "SLightActorDetailPanel.h"
#include "SSettingsView.h"

/**
 *
 */
class SLightItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLightItem)
	{
	}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

public:
	TSharedPtr<SLightActorComboBox> ComboBox;
	TSharedPtr<SLightActorDetailPanel> DetailPanel;
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLightItem::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(2)
		.BorderImage(FEditorStyle::GetBrush("ToolBar.Background"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SAssignNew(ComboBox, SLightActorComboBox)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString("DELETE"))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(DetailPanel, SLightActorDetailPanel)
			]
		]
	];
}

void SLightActorGroup::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(MainLayout, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SAssignNew(TitleBlock, STextBlock)
				.Text(FText::FromString("Other Light"))
				.TextStyle(FEditorStyle::Get(), "LargeText")
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SSpacer)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("CLEAR"))
				.OnClicked_Lambda([this]() -> FReply {
					Clear();
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("APPEND"))
				.OnClicked_Lambda([this]() -> FReply {
					OnAddLightItem();
					return FReply::Handled();
				})
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(Group, SVerticalBox)
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLightActorGroup::Clear()
{
	for (auto Widget : LightItemWidgets) {
		auto RemoveSlotIndex = Group->RemoveSlot(Widget);
		ensure(RemoveSlotIndex >= 0);
	}
	LightItemWidgets.Reset();
	DataField = nullptr;
}

void SLightActorGroup::OnAddLightItem()
{
	if (DataField) {
		AddLightItemWidget();
		AddLightItemDatafield();
	}
}

void SLightActorGroup::OnSolutionChanged(int SolutionIndex)
{
	Clear();
	if (USceneManagementAsset* SceneManagementAsset = SSettingsView::GetSceneManagementAsset()) {
		if (SolutionIndex < 0) {
			return;
		}
		DataField = SceneManagementAsset->GetAuxLightGroupsPtr(SolutionIndex, (ELightCategory)LightCategory);
		for (auto LightParams : DataField->Array) {
			AddLightItemWidget();
		}
	}
}

void SLightActorGroup::AddLightItemWidget()
{
	TSharedRef<SLightItem> Widget = SNew(SLightItem);
	Group->AddSlot().AutoHeight()[Widget];
	LightItemWidgets.Add(Widget);
}

void SLightActorGroup::AddLightItemDatafield()
{
	if (DataField) {
		DataField->AddLightParam();
	}
}
