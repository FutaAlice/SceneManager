// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneManager.h"

#include "EditorStyleSet.h" // FEditorStyle
#include "Styling/CoreStyle.h"  // FCoreStyle
#include "Framework/Application/SlateApplication.h" // FSlateApplication
#include "Framework/Commands/Commands.h"    // TCommands
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs, FGlobalTabmanager
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "WorkspaceMenuStructure.h" // IWorkspaceMenuStructure
#include "WorkspaceMenuStructureModule.h"   // WorkspaceMenu


#define LOCTEXT_NAMESPACE "FSceneManagerModule"

FSceneManagerModule::FSceneManagerModule()
    : Impl(MakeUnique<FSceneManagerImpl>())
{
}

void FSceneManagerModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSceneManagerModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

struct FSceneManagerCommands : public TCommands<FSceneManagerCommands>
{
    FSceneManagerCommands()
        : TCommands<FSceneManagerCommands>(
            TEXT("SceneManager"), // Context name for fast lookup
            NSLOCTEXT("Contexts", "SceneManager", "SceneManager Plugin"), // Localized context name for displaying
            NAME_None, // Parent
            FCoreStyle::Get().GetStyleSetName() // Icon Style Set
            )
    {
    }

    // TCommand<> interface
    virtual void RegisterCommands() override;
    // End of TCommand<> interface

    TSharedPtr<FUICommandInfo> Action_A;
    TSharedPtr<FUICommandInfo> Action_B;
};

void FSceneManagerCommands::RegisterCommands()
{
    UI_COMMAND(Action_A, "Call ", "Toggles visibility of the Call Stack window", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(Action_B, "Watches", "Toggles visibility of the Watches window", EUserInterfaceActionType::Check, FInputChord());
}

struct FSceneManagerImpl
{
    FSceneManagerImpl();
    ~FSceneManagerImpl();

    /** Function registered with tab manager to create the bluepring debugger */
    TSharedRef<SDockTab> CreateSceneManagerTab(const FSpawnTabArgs& Args);

    TSharedPtr<FTabManager> DebuggingToolsTabManager;
    TSharedPtr<FTabManager::FLayout> SceneManagerLayout;

private:
    // prevent copying:
    FSceneManagerImpl(const FSceneManagerImpl &) = delete;
    FSceneManagerImpl(FSceneManagerImpl &&) = delete;
    FSceneManagerImpl& operator=(FSceneManagerImpl const &) = delete;
    FSceneManagerImpl& operator=(FSceneManagerImpl &&) = delete;
};

const FName SceneManagerName = FName(TEXT("SceneManager"));

FSceneManagerImpl::FSceneManagerImpl()
{
    const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();

    FSceneManagerCommands::Register();
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SceneManagerName, FOnSpawnTab::CreateRaw(this, &FSceneManagerImpl::CreateSceneManagerTab))
        .SetDisplayName(LOCTEXT("FSceneManagerTabTitle", "Scene Manager"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Manager tab."))
        .SetGroup(MenuStructure.GetDeveloperToolsMiscCategory())
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "BlueprintDebugger.TabIcon")); // TODO
}

FSceneManagerImpl::~FSceneManagerImpl()
{
    if (FSlateApplication::IsInitialized()) {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SceneManagerName);
    }

    if (DebuggingToolsTabManager.IsValid()) {
        FGlobalTabmanager::Get()->UnregisterTabSpawner(SceneManagerName);
        SceneManagerLayout = TSharedPtr<FTabManager::FLayout>();
        DebuggingToolsTabManager = TSharedPtr<FTabManager>();
    }
    FSceneManagerCommands::Unregister();
}

TSharedRef<SDockTab> FSceneManagerImpl::CreateSceneManagerTab(const FSpawnTabArgs& Args)
{
    const TSharedRef<SDockTab> NomadTab = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(NSLOCTEXT("BlueprintDebugger", "TabTitle", "Blueprint Debugger"));

    return NomadTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSceneManagerModule, SceneManager)
