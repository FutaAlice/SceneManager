#include "SCharacterLightingViewer.h"
#include "SlateOptMacros.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs
#include "EditorStyleSet.h" // FEditorStyle
#include "Widgets/Docking/SDockTab.h"   // SDockTab

#define LOCTEXT_NAMESPACE "CharacterLightViewer"

/**
 *
 */
class SCharacterLightingViewer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCharacterLightingViewer)
	{
	}
	SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
};


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCharacterLightingViewer::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace CharacterLightingViewer {

FName GetTabName()
{
    const FName TabName = TEXT("CharacterLightingViewer");
    return TabName;
}

void RegisterTabSpawner(FTabManager& TabManager)
{
    const auto SpawnCharacterLightingViewTab = [](const FSpawnTabArgs& Args) {
        return SNew(SDockTab)
            .TabRole(ETabRole::PanelTab)
            .Label(LOCTEXT("TabTitle", "Character Lighting"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
            [
                SNew(SCharacterLightingViewer/*, &Private_MaterialSource*/)
            ]
            ];
    };

    TabManager.RegisterTabSpawner(CharacterLightingViewer::GetTabName(), FOnSpawnTab::CreateStatic(SpawnCharacterLightingViewTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Scene Lighting"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Lighting tab"));
}

void OnAssetDataChanged();

} // namespace SceneLightingViewer

#undef LOCTEXT_NAMESPACE
