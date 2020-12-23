#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace PostProcessViewer {

FName GetTabName(int LightCategory);
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace LightingViewer
