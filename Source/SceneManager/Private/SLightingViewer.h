#pragma once
#include "CoreMinimal.h"

class FTabManager;
class USceneManagementAssetData;

namespace LightingViewer {

FName GetTabName(int LightCategory);
void RegisterTabSpawner(FTabManager& TabManager, int LightCategory);

// Callback functions for SSettingView
void OnAssetDataChanged(USceneManagementAssetData* AssetData);
void OnMPCChanged();

// void DebugSyncLightingSolutionRename(int SolutionIndex, FString SolutionName);

} // namespace LightingViewer
