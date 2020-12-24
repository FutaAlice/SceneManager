#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace PostProcessViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace LightingViewer
