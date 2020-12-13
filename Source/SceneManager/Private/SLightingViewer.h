#pragma once
#include "CoreMinimal.h"

class FTabManager;
class UMaterialParameterCollection;
class USceneManagementAssetData;

namespace LightingViewer {

FName GetTabName(int LightCategory);
void RegisterTabSpawner(FTabManager& TabManager, int LightCategory);

// Callback functions for SSettingView
void OnAssetDataChanged(USceneManagementAssetData* AssetData);
void OnMPCChanged(UMaterialParameterCollection* MPC, int LightCategory);

// void DebugSyncLightingSolutionRename(int SolutionIndex, FString SolutionName);

} // namespace LightingViewer
