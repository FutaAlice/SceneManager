#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace SceneLightingViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace SceneLightingViewer
