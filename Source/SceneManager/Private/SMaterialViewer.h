#pragma once
#include "CoreMinimal.h"

class FTabManager;

namespace MaterialViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace MaterialViewer