// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneManager.h"

#include "EditorStyleSet.h" // FEditorStyle
#include "Styling/CoreStyle.h"  // FCoreStyle
#include "Framework/Application/SlateApplication.h" // FSlateApplication
#include "Framework/Commands/Commands.h"    // TCommands
#include "Framework/Commands/UICommandList.h"   // FUICommandList
#include "Framework/Docking/TabManager.h"   // FTabManager, FSpawnTabArgs, FGlobalTabmanager
#include "Framework/Docking/LayoutService.h"    // FLayoutSaveRestore
#include "Framework/MultiBox/MultiBoxBuilder.h" // FMenuBuilder
#include "Widgets/Docking/SDockTab.h"   // SDockTab
#include "WorkspaceMenuStructure.h" // IWorkspaceMenuStructure
#include "WorkspaceMenuStructureModule.h"   // WorkspaceMenu

#include "SSettingsView.h"
#include "SMaterialViewer.h"
#include "SSceneLightingViewer.h"


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

/**
 * @brief A set of actions of Scene Manager main frame
 * 
 */
struct FSceneManagerCommands : public TCommands<FSceneManagerCommands>
{
    FSceneManagerCommands()
        : TCommands<FSceneManagerCommands>(
            TEXT("SceneManager"), // Context name for fast lookup
            LOCTEXT("SceneManager", "SceneManager Plugin"), // Localized context name for displaying
            NAME_None, // Parent
            FCoreStyle::Get().GetStyleSetName() // Icon Style Set
            )
    {
    }

    // TCommand<> interface
    virtual void RegisterCommands() override;
    // End of TCommand<> interface

    TSharedPtr<FUICommandInfo> TabAction_Meterial;
    TSharedPtr<FUICommandInfo> TabAction_SceneLighting;
    TSharedPtr<FUICommandInfo> TabAction_CharacterLighting;
    TSharedPtr<FUICommandInfo> TabAction_PostProcessing;
    TSharedPtr<FUICommandInfo> TabAction_Wind;
    TSharedPtr<FUICommandInfo> TabAction_Misc;
    TSharedPtr<FUICommandInfo> TabAction_Settings;
};

void FSceneManagerCommands::RegisterCommands()
{
    // tab pages name
    UI_COMMAND(TabAction_Meterial, "Meterial", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_SceneLighting, "Scene Lighting", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_CharacterLighting, "Character Lighting", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_PostProcessing, "Post-Processing", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_Wind, "Wind", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_Misc, "Misc", "TODO", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(TabAction_Settings, "Settings", "Scene manager settings", EUserInterfaceActionType::Check, FInputChord());
}

/**
 * @brief Toggle main frame pages
 * 
 */
struct FSceneManagerImpl
{
    FSceneManagerImpl();
    ~FSceneManagerImpl();

    /** Function registered with tab manager to create the scene manager window */
    TSharedRef<SDockTab> CreateSceneManagerTab(const FSpawnTabArgs& Args);

    TSharedPtr<FTabManager> ToolsTabManager;
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
        .SetDisplayName(LOCTEXT("TabTitle", "Scene Manager"))
        .SetTooltipText(LOCTEXT("TooltipText", "Open the Scene Manager tab."))
        .SetGroup(MenuStructure.GetDeveloperToolsMiscCategory())
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "BlueprintDebugger.TabIcon")); // TODO: change icon
}

FSceneManagerImpl::~FSceneManagerImpl()
{
    if (FSlateApplication::IsInitialized()) {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SceneManagerName);
    }

    if (ToolsTabManager.IsValid()) {
        FGlobalTabmanager::Get()->UnregisterTabSpawner(SceneManagerName);
        SceneManagerLayout = TSharedPtr<FTabManager::FLayout>();
        ToolsTabManager = TSharedPtr<FTabManager>();
    }
    FSceneManagerCommands::Unregister();
}

TSharedRef<SDockTab> FSceneManagerImpl::CreateSceneManagerTab(const FSpawnTabArgs& Args)
{
    const TSharedRef<SDockTab> NomadTab = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(LOCTEXT("TabTitle", "Scene Manager"));

    if (!ToolsTabManager.IsValid()) {
        ToolsTabManager = FGlobalTabmanager::Get()->NewTabManager(NomadTab);
        // on persist layout will handle saving layout if the editor is shut down:
        ToolsTabManager->SetOnPersistLayout(
            FTabManager::FOnPersistLayout::CreateStatic(
                [](const TSharedRef<FTabManager::FLayout>& InLayout) {
                    if (InLayout->GetPrimaryArea().Pin().IsValid()) {
                        // FLayoutSaveRestore::SaveToConfig(GEditorLayoutIni, InLayout);
                    }
                }
            )
        );
    }
    else {
        ensure(SceneManagerLayout.IsValid());
    }

    TWeakPtr<FTabManager> ToolsTabManagerWeak = ToolsTabManager;
    // On tab close will save the layout if the debugging window itself is closed,
    // this handler also cleans up any floating debugging controls. If we don't close
    // all areas we need to add some logic to the tab manager to reuse existing tabs:
    NomadTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateStatic(
        [](TSharedRef<SDockTab> Self, TWeakPtr<FTabManager> TabManager) {
            TSharedPtr<FTabManager> OwningTabManager = TabManager.Pin();
            if (OwningTabManager.IsValid()) {
                // FLayoutSaveRestore::SaveToConfig(GEditorLayoutIni, OwningTabManager->PersistLayout());
                OwningTabManager->CloseAllAreas();
            }
        }
        , ToolsTabManagerWeak));

    const FName TabName_Meterial = MaterialViewer::GetTabName();
    const FName TabName_SceneLighting = SceneLightingViewer::GetTabName();
    const FName TabName_CharacterLighting = FName(TEXT("Character Lighting"));
    const FName TabName_PostProcessing = FName(TEXT("Post-Processing"));
    const FName TabName_Wind = FName(TEXT("Wind"));
    const FName TabName_Misc = FName(TEXT("Miscellaneous"));
    const FName TabName_Settings = FName(TEXT("Settings"));

    if (!SceneManagerLayout.IsValid()) {
        ToolsTabManager->RegisterTabSpawner(
            TabName_Settings,
            FOnSpawnTab::CreateStatic(
                [](const FSpawnTabArgs&)->TSharedRef<SDockTab> {
                    return SNew(SDockTab)
                        .TabRole(ETabRole::PanelTab)
                        .Label(NSLOCTEXT("SceneManagerSettings", "TabTitle", "Settings"))
                        [
                             SNew(SSettingsView)
                        ];
                }
            ))
            .SetDisplayName(LOCTEXT("SettingsTabTitle", "Scenc manager settings"))
            .SetTooltipText(LOCTEXT("SettingsTooltipText", "Open the Scenc manager setting tab."));

        MaterialViewer::RegisterTabSpawner(*ToolsTabManager);
        SceneLightingViewer::RegisterTabSpawner(*ToolsTabManager);
        //WatchViewer::RegisterTabSpawner(*DebuggingToolsTabManager);

        SceneManagerLayout = FTabManager::NewLayout("Standalone_SceneManager_Layout_v3");
        SceneManagerLayout->AddArea(
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split(
                FTabManager::NewStack()
                ->SetSizeCoefficient(.4f)
                ->SetHideTabWell(true)
                ->AddTab(TabName_Meterial, ETabState::OpenedTab)
                ->AddTab(TabName_SceneLighting, ETabState::OpenedTab)
                ->AddTab(TabName_CharacterLighting, ETabState::OpenedTab)
                ->AddTab(TabName_PostProcessing, ETabState::OpenedTab)
                ->AddTab(TabName_Wind, ETabState::OpenedTab)
                ->AddTab(TabName_Misc, ETabState::OpenedTab)
                ->AddTab(TabName_Settings, ETabState::OpenedTab)
                ->SetForegroundTab(TabName_Settings)
            )
        );
    }

    SceneManagerLayout = FLayoutSaveRestore::LoadFromConfig(GEditorLayoutIni, SceneManagerLayout.ToSharedRef());

    TSharedRef<SWidget> TabContents = ToolsTabManager->RestoreFrom(SceneManagerLayout.ToSharedRef(), TSharedPtr<SWindow>()).ToSharedRef();

    // build command list for tab restoration menu:
    TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());

    TWeakPtr<FTabManager> ToolsManagerWeak = ToolsTabManager;

    const auto ToggleTabVisibility = [](TWeakPtr<FTabManager> InTabManagerWeak, FName InTabName) {
        TSharedPtr<FTabManager> InTabManager = InTabManagerWeak.Pin();
        if (InTabManager.IsValid()) {
            TSharedPtr<SDockTab> ExistingTab = InTabManager->FindExistingLiveTab(InTabName);
            if (ExistingTab.IsValid()) {
                ExistingTab->RequestCloseTab();
            }
            else {
                InTabManager->InvokeTab(InTabName);
            }
        }
    };

    const auto IsTabVisible = [](TWeakPtr<FTabManager> InTabManagerWeak, FName InTabName) {
        TSharedPtr<FTabManager> InTabManager = InTabManagerWeak.Pin();
        if (InTabManager.IsValid()) {
            return InTabManager->FindExistingLiveTab(InTabName).IsValid();
        }
        return false;
    };

    CommandList->MapAction(
        FSceneManagerCommands::Get().TabAction_Settings,
        FExecuteAction::CreateStatic(ToggleTabVisibility, ToolsManagerWeak, TabName_Settings),
        FCanExecuteAction::CreateStatic([] { return true; }),
        FIsActionChecked::CreateStatic(IsTabVisible, ToolsManagerWeak, TabName_Settings)
    );

    TWeakPtr<SWidget> OwningWidgetWeak = NomadTab;
    //TabContents->SetOnMouseButtonUp(
    //    FPointerEventHandler::CreateStatic(
    //        [](
    //            const FGeometry&,   // The geometry of the widget
    //            const FPointerEvent& PointerEvent,  // The Mouse Event that we are processing
    //            TWeakPtr<SWidget> InOwnerWeak,
    //            TSharedPtr<FUICommandList> InCommandList) -> FReply {
    //                if (PointerEvent.GetEffectingButton() == EKeys::RightMouseButton) {
    //                    // if the tab manager is still available then make a context window that allows users to
    //                    // show and hide tabs:
    //                    TSharedPtr<SWidget> InOwner = InOwnerWeak.Pin();
    //                    if (InOwner.IsValid()) {
    //                        FMenuBuilder MenuBuilder(true, InCommandList);

    //                        // add tab pages
    //                        MenuBuilder.PushCommandList(InCommandList.ToSharedRef());
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_Meterial);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_SceneLighting);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_CharacterLighting);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_PostProcessing);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_Wind);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_Misc);
    //                        MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_Settings);
    //                        MenuBuilder.PopCommandList();

    //                        FWidgetPath WidgetPath = PointerEvent.GetEventPath() != nullptr ? *PointerEvent.GetEventPath() : FWidgetPath();
    //                        FSlateApplication::Get().PushMenu(InOwner.ToSharedRef(), WidgetPath, MenuBuilder.MakeWidget(), PointerEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

    //                        return FReply::Handled();
    //                    }
    //                }
    //                return FReply::Unhandled();
    //        }
    //        , OwningWidgetWeak , CommandList));

    NomadTab->SetContent(
        SNew(SBorder)
        .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
        .Padding(FMargin(0.f, 2.f))
        [
            TabContents
        ]
    );


    return NomadTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSceneManagerModule, SceneManager)
