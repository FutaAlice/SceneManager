#pragma once

#include "CoreMinimal.h"

class FTabManager;

namespace CharacterLightingViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);
void OnAssetDataChanged();

} // namespace SceneLightingViewer
