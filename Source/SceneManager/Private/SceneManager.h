// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Templates/UniquePtr.h"

class FSceneManagerModule : public IModuleInterface
{
public:
    FSceneManagerModule();

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
private:
    TUniquePtr<struct FSceneManagerImpl> Impl;
};
