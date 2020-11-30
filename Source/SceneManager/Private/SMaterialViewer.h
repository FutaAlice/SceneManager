// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Widgets/SCompoundWidget.h"

class FTabManager;

namespace MaterialViewer {

FName GetTabName();
void RegisterTabSpawner(FTabManager& TabManager);

} // namespace MaterialViewer
