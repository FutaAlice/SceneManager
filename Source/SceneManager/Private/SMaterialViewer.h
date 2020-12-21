#pragma once
#include "CoreMinimal.h"

class FTabManager;
class USceneManagementAssetData;

namespace MaterialViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

void OnAssetDataChanged(USceneManagementAssetData* AssetData);
void OnEditorModified();

} // namespace MaterialViewer
