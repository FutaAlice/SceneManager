#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace SceneLightingViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);
void OnAssetDataChanged();

} // namespace SceneLightingViewer
