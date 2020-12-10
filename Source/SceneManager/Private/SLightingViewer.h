#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace LightingViewer {

FName GetTabName(int LightCategory);
void RegisterTabSpawner(FTabManager& TabManagerk, int LightCategory);

void OnAssetDataChanged();
void OnMPCChanged();

void DebugSyncLightingSolutionRename(int SolutionIndex, FString SolutionName);

} // namespace LightingViewer
