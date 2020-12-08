#include "SceneManagementAssetActions.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"


SceneManagementAssetActions::SceneManagementAssetActions(EAssetTypeCategories::Type InAssetCategory)
    : MyAssetCategory(InAssetCategory)
{
}

FColor SceneManagementAssetActions::GetTypeColor() const
{
    return FColor::Red;
}

void SceneManagementAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

void SceneManagementAssetActions::RegisterAssetTypeActions()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    EAssetTypeCategories::Type AssetTypeCategory = EAssetTypeCategories::Misc;
    TSharedPtr<IAssetTypeActions> ActionType = MakeShareable(new SceneManagementAssetActions(AssetTypeCategory));

    AssetTools.RegisterAssetTypeActions(ActionType.ToSharedRef());
}
