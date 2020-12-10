#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "SceneManagementAsset.h"

class SceneManagementAssetActions : public FAssetTypeActions_Base
{
public:
    SceneManagementAssetActions(EAssetTypeCategories::Type InAssetCategory);

    virtual FColor GetTypeColor() const override;
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

    // IAssetTypeActions Implementation
    virtual FText GetName() const override { return FText::FromName(TEXT("Scene Management Asset")); }
    virtual UClass* GetSupportedClass() const override { return USceneManagementAsset::StaticClass(); }
    virtual uint32 GetCategories() override { return MyAssetCategory; }

    static void RegisterAssetTypeActions();

private:
    EAssetTypeCategories::Type MyAssetCategory;
};
