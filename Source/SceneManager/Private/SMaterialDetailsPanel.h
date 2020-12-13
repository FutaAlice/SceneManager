// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UMaterialInstance;
class UMaterialInfo;
class IDetailsView;
class SUniformGridPanel;
class SBoxPanel;
class SColorBlock;

/**
 * 
 */
class SMaterialDetailsPanel : public SCompoundWidget
{
private:
    TSharedRef<SBoxPanel> CreateVectorParamSlot(FString Key, FLinearColor Value, TSharedPtr<SColorBlock> ColorBlock);
    FReply OnClickColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, FString Name);
    void OnColorCommitted(FLinearColor NewColor, FString Name);
    void OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window);

public:
	SLATE_BEGIN_ARGS(SMaterialDetailsPanel)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

    void BindDataField(UObject* InObject);
    UMaterialInfo* GetDataField();
    void ForceRefresh();

private:
    void ResetDataBySelectedAsset();
    void SyncToGridPanel();
    void OnFinishedChangingProperties(const FPropertyChangedEvent& InEvent);

    FLinearColor GetVectorByName(FString Name) const;

private:
    UMaterialInfo* MaterialInfo;
    IDetailsView* DetailView;
    TSharedPtr<SUniformGridPanel> UniformGridPanel;
};
