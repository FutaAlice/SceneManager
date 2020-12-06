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

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLightActorGroup::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(MainLayout, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 2)
		[
			SAssignNew(TitleBlock, STextBlock)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ToolBarContainer, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("TODO"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(Groups, SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Padding"))
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
