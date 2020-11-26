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
    TSharedPtr<FUICommandInfo> TabAction_Settings;
};

void FSceneManagerCommands::RegisterCommands()
{
    UI_COMMAND(Action_A, "Call ", "Toggles visibility of the Call Stack window", EUserInterfaceActionType::Check, FInputChord());
    UI_COMMAND(Action_B, "Watches", "Toggles visibility of the Watches window", EUserInterfaceActionType::Check, FInputChord());
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

    /** Function registered with tab manager to create the bluepring debugger */
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
        .Label(LOCTEXT("TabTitle", "Blueprint Debugger"));

    if (!ToolsTabManager.IsValid()) {
        ToolsTabManager = FGlobalTabmanager::Get()->NewTabManager(NomadTab);
        // on persist layout will handle saving layout if the editor is shut down:
        ToolsTabManager->SetOnPersistLayout(
            FTabManager::FOnPersistLayout::CreateStatic(
                [](const TSharedRef<FTabManager::FLayout>& InLayout) {
                    if (InLayout->GetPrimaryArea().Pin().IsValid()) {
                        FLayoutSaveRestore::SaveToConfig(GEditorLayoutIni, InLayout);
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
                FLayoutSaveRestore::SaveToConfig(GEditorLayoutIni, OwningTabManager->PersistLayout());
                OwningTabManager->CloseAllAreas();
            }
        }
        , ToolsTabManagerWeak));

    const FName Action_AName = FName(TEXT("My Action A"));
    const FName Action_BName = FName(TEXT("My Action B"));
    const FName TabName_Settings = FName(TEXT("Settings"));

    if (!SceneManagerLayout.IsValid()) {
        ToolsTabManager->RegisterTabSpawner(
            TabName_Settings,
            FOnSpawnTab::CreateStatic(
                [](const FSpawnTabArgs&)->TSharedRef<SDockTab> {
                    return SNew(SDockTab)
                        .TabRole(ETabRole::PanelTab)
                        .Label(LOCTEXT("TabTitle", "Settings"))
                        [
                             SNew(SSettingsView)
                        ];
                }
            ))
            .SetDisplayName(LOCTEXT("SettingsTabTitle", "Scenc manager settings"))
            .SetTooltipText(LOCTEXT("SettingsTooltipText", "Open the Scenc manager setting tab."));

        //CallStackViewer::RegisterTabSpawner(*DebuggingToolsTabManager);
        //WatchViewer::RegisterTabSpawner(*DebuggingToolsTabManager);

        SceneManagerLayout = FTabManager::NewLayout("Standalone_SceneManager_Layout_v1");
        SceneManagerLayout->AddArea(
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split(
                FTabManager::NewStack()
                ->SetSizeCoefficient(.4f)
                ->SetHideTabWell(true)
                ->AddTab(Action_AName, ETabState::OpenedTab)
                ->AddTab(Action_BName, ETabState::OpenedTab)
                ->AddTab(TabName_Settings, ETabState::OpenedTab)
                ->SetForegroundTab(Action_AName)
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
        FExecuteAction::CreateStatic(
            ToggleTabVisibility, ToolsManagerWeak, TabName_Settings
        ),
        FCanExecuteAction::CreateStatic(
            []() { return true; }
        ),
        FIsActionChecked::CreateStatic(
            IsTabVisible, ToolsManagerWeak, TabName_Settings
        )
    );

    TWeakPtr<SWidget> OwningWidgetWeak = NomadTab;
    TabContents->SetOnMouseButtonUp(
        FPointerEventHandler::CreateStatic(
            [](
                const FGeometry&,   // The geometry of the widget
                const FPointerEvent& PointerEvent,  // The Mouse Event that we are processing
                TWeakPtr<SWidget> InOwnerWeak,
                TSharedPtr<FUICommandList> InCommandList) -> FReply {
                    if (PointerEvent.GetEffectingButton() == EKeys::RightMouseButton) {
                        // if the tab manager is still available then make a context window that allows users to
                        // show and hide tabs:
                        TSharedPtr<SWidget> InOwner = InOwnerWeak.Pin();
                        if (InOwner.IsValid()) {
                            FMenuBuilder MenuBuilder(true, InCommandList);

                            MenuBuilder.PushCommandList(InCommandList.ToSharedRef());
                            MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().Action_A);
                            MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().Action_B);
                            MenuBuilder.AddMenuEntry(FSceneManagerCommands::Get().TabAction_Settings);
                            MenuBuilder.PopCommandList();

                            FWidgetPath WidgetPath = PointerEvent.GetEventPath() != nullptr ? *PointerEvent.GetEventPath() : FWidgetPath();
                            FSlateApplication::Get().PushMenu(InOwner.ToSharedRef(), WidgetPath, MenuBuilder.MakeWidget(), PointerEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

                            return FReply::Handled();
                        }
                    }
                    return FReply::Unhandled();
            }
            , OwningWidgetWeak , CommandList));

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
