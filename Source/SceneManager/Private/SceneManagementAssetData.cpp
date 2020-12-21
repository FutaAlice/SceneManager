#include "SceneManagementAssetData.h"

#include "Editor.h" // GEditor
#include "Engine/Level.h"   // ULevel
#include "Engine/Light.h"   // ALight
#include "Engine/World.h"   // UWorld
#include "Components/LightComponent.h"  // ULightComponent
#include "Materials/MaterialInstanceConstant.h" // UMaterialInstanceConstant
#include "Kismet/GameplayStatics.h" // UGameplayStatics
#include "Misc/MessageDialog.h" // FMessageDialog

#include "SSettingsView.h"

void ULightParams::Clear()
{
    LightActor = nullptr;
    ActorName = "";
    Rotation = FRotator();
    Intensity = 0;
    LightColor = FLinearColor();
}

void ULightParams::FromActor()
{
    if (LightActor) {
        ActorName = LightActor->GetName();
        // Rotation = LightActor->GetLightComponent()->GetComponentRotation();
        Rotation = LightActor->GetActorRotation();
        Intensity = LightActor->GetLightComponent()->Intensity;
        LightColor = LightActor->GetLightColor();
    }
}

void ULightParams::ToActor()
{
    if (LightActor) {
        //LightActor->GetLightComponent()->SetWorldRotation(Rotation);
        LightActor->SetActorRotation(Rotation);
        LightActor->GetLightComponent()->SetIntensity(Intensity);
        LightActor->SetLightColor(LightColor);
    }
}

ULightParams* UGroupLightParams::AddLightParam()
{
    ULightParams *LightParams = NewObject<ULightParams>(this);
    Array.Add(LightParams);
    return LightParams;
}

void UGroupLightParams::RemoveLightParam(ULightParams* LightParams)
{
    int Index = Array.Find(LightParams);
    ensure(Index >= 0);
    Array.RemoveAt(Index);
}

void UMaterialInfo::ClearParams()
{
    VectorParams.Empty();
    ScalarParams.Empty();
}

void UMaterialInfo::FromMaterial()
{
    ClearParams();

    if (UMaterialInstance* Material = GetMaterial<UMaterialInstance>()) {
        TArray<FMaterialParameterInfo> ParameterInfos;
        TArray<FGuid> ParameterGuids;

        Material->GetAllScalarParameterInfo(ParameterInfos, ParameterGuids);
        for (auto MaterialParameterInfo : ParameterInfos) {
            FString Key = MaterialParameterInfo.Name.ToString();
            float Value;
            Material->GetScalarParameterValue(MaterialParameterInfo, Value);
            ScalarParams.Add(Key, Value);
        }
        ParameterInfos.Empty();
        ParameterGuids.Empty();

        Material->GetAllVectorParameterInfo(ParameterInfos, ParameterGuids);
        for (auto MaterialParameterInfo : ParameterInfos) {
            FString Key = MaterialParameterInfo.Name.ToString();
            FLinearColor Value;
            Material->GetVectorParameterValue(MaterialParameterInfo, Value);
            VectorParams.Add(Key, Value);
        }
    }
}

void UMaterialInfo::ToMaterial()
{
    if (UMaterialInstanceConstant* Material = GetMaterial<UMaterialInstanceConstant>()) {
        //Get scalar parameters name and value
        TArray<FMaterialParameterInfo> ParameterInfos;
        TArray<FGuid> ParameterGuids;
        Material->GetAllVectorParameterInfo(ParameterInfos, ParameterGuids);

        for (size_t i = 0; i < ParameterInfos.Num(); i++) {
            FString ParamName = ParameterInfos[i].Name.ToString();
            if (FLinearColor *Color = VectorParams.Find(ParamName)) {
                Material->SetVectorParameterValueEditorOnly(ParameterInfos[i], *Color);
            }
        }
    }
}

void USceneManagementAssetData::AddLightingSolution()
{
    LightingSolutionNameList.Add("");

    KeyLightParams.Add(NewObject<ULightParams>(this));
    SceneAuxGroups.Add(NewObject<UGroupLightParams>(this));
    CharacterAuxGroups.Add(NewObject<UGroupLightParams>(this));

    SyncActorByName();
}

void USceneManagementAssetData::DuplicateLightingSolution(int SolutionIndex)
{
    int TargetIndex = LightingSolutionNameList.Num() - 1;
    KeyLightParams[TargetIndex] = DuplicateObject<ULightParams>(KeyLightParams[SolutionIndex], this);

    for (ULightParams* LightParams : SceneAuxGroups[SolutionIndex]->Array) {
        SceneAuxGroups[TargetIndex]->Array.Add(DuplicateObject<ULightParams>(LightParams, this));
    }
    for (ULightParams* LightParams : CharacterAuxGroups[SolutionIndex]->Array) {
        CharacterAuxGroups[TargetIndex]->Array.Add(DuplicateObject<ULightParams>(LightParams, this));
    }

    SyncActorByName();
}

void USceneManagementAssetData::RemoveLightingSolution(int SolutionIndex)
{
    LightingSolutionNameList.RemoveAt(SolutionIndex);
    KeyLightParams.RemoveAt(SolutionIndex);
    SceneAuxGroups.RemoveAt(SolutionIndex);
    CharacterAuxGroups.RemoveAt(SolutionIndex);
}

void USceneManagementAssetData::RenameLightingSolution(int SolutionIndex, const FString& SolutionName)
{
    LightingSolutionNameList[SolutionIndex] = SolutionName;
}

ULightParams* USceneManagementAssetData::GetKeyLightParamsPtr(int SolutionIndex)
{
    ensure(KeyLightParams.Num() > SolutionIndex);
    return KeyLightParams[SolutionIndex];
}

UGroupLightParams* USceneManagementAssetData::GetAuxLightGroupsPtr(int SolutionIndex, int LightCategory)
{
    TArray<UGroupLightParams*>* GroupArray = nullptr;
    if (LightCategory & LightCategory_SceneLight) {
        GroupArray = &SceneAuxGroups;
    }
    else if (LightCategory & LightCategory_CharacterLight) {
        GroupArray = &CharacterAuxGroups;
    }

    ensure(GroupArray);
    ensure(GroupArray->Num() > SolutionIndex);
    return (*GroupArray)[SolutionIndex];
}

bool USceneManagementAssetData::GetGroupRange(FString GroupName, int & BeginIndex, int & EndIndex)
{
    int GroupIndex = MaterialGroupNameList.Find(GroupName);
    if (GroupIndex == INDEX_NONE) {
        BeginIndex = EndIndex = 0;
        return false;
    }

    BeginIndex = MaterialGroupIndexList[GroupIndex];
    EndIndex = (GroupIndex == MaterialGroupNameList.Num() - 1) ?   // is the last group ?
        MaterialSolutions[0]->SolutionItems.Num() :
        MaterialGroupIndexList[GroupIndex + 1];
    return true;
}

void USceneManagementAssetData::AddMaterialSolution()
{
    MaterialSolutionNameList.Add("");
    MaterialSolutions.Add(NewObject<USolutionMaterialInfo>(this));

    // first time, create default group
    if (MaterialGroupNameList.Num() == 0) {
        MaterialGroupNameList.Add("Default");
        MaterialGroupIndexList.Add(0);
    }
    // otherwise, copy SoftObjectPath
    else {
        ensure(MaterialSolutions.Num() > 1);
        USolutionMaterialInfo* From = MaterialSolutions[0];
        USolutionMaterialInfo* To = MaterialSolutions.Last();

        for (auto MaterialInfo : From->SolutionItems) {
            UMaterialInfo *NewItem = NewObject<UMaterialInfo>(this);
            NewItem->SoftObjectPath = MaterialInfo->SoftObjectPath;
            NewItem->FromMaterial();
            To->SolutionItems.Add(NewItem);
        }
    }

    // TODO
    SyncMaterialByName();
}

void USceneManagementAssetData::AddMaterialGroup()
{
    // generate a group name
    FString GroupName;
    for (int i = MaterialGroupNameList.Num();;) {
        GroupName = FString::Printf(TEXT("NUM %d"), ++i);
        if (MaterialGroupNameList.Find(GroupName) == INDEX_NONE) {
            break;
        }
    }

    ensure(MaterialSolutions.Num() > 0);
    MaterialGroupNameList.Add(GroupName);
    MaterialGroupIndexList.Add(MaterialSolutions[0]->SolutionItems.Num());
}

void USceneManagementAssetData::AddMaterial(FString GroupName, FSoftObjectPath DefaultValue)
{
    if (MaterialSolutions.Num() <= 0) {
        return;
    }

    int GroupIndex = MaterialGroupNameList.Find(GroupName);
    if (GroupIndex == INDEX_NONE) {
        return;
    }

    int Unused, EndIndex;
    GetGroupRange(GroupName, Unused, EndIndex);

    // all index after this gourp name +1
    for (int i = GroupIndex + 1; i < MaterialGroupNameList.Num(); ++i) {
        MaterialGroupIndexList[i] += 1;
    }

    for (auto SO : MaterialSolutions) {
        UMaterialInfo* MaterialInfo = NewObject<UMaterialInfo>(this);
        MaterialInfo->SoftObjectPath = DefaultValue;
        MaterialInfo->FromMaterial();
        SO->SolutionItems.Insert(MaterialInfo, EndIndex);
    }
}

void USceneManagementAssetData::RemoveMaterialSolution(int SolutionIndex)
{
    MaterialSolutionNameList.RemoveAt(SolutionIndex);
    MaterialSolutions.RemoveAt(SolutionIndex);
}

void USceneManagementAssetData::RemoveMaterialGroup(FString GroupName)
{
    int GroupIndex = MaterialGroupNameList.Find(GroupName);
    if (GroupIndex == INDEX_NONE) {
        return;
    }

    int BeginIndex, EndIndex;
    GetGroupRange(GroupName, BeginIndex, EndIndex);

    // remove group label and index
    MaterialGroupNameList.RemoveAt(GroupIndex);
    MaterialGroupIndexList.RemoveAt(GroupIndex);

    // fix other group indices
    const int RemoveCount = EndIndex - BeginIndex;
    for (int i = GroupIndex; i < MaterialGroupIndexList.Num(); ++i) {
        MaterialGroupIndexList[i] -= RemoveCount;
    }

    // remove item data
    for (auto SolutionMaterialInfo : MaterialSolutions) {
        SolutionMaterialInfo->SolutionItems.RemoveAt(BeginIndex, RemoveCount);
    }
}

void USceneManagementAssetData::RenameMaterialSolution(int SolutionIndex, const FString & SolutionName)
{
    MaterialSolutionNameList[SolutionIndex] = SolutionName;
}

bool USceneManagementAssetData::RenameMaterialGroup(FString OldName, FString NewName)
{
    int GroupIndex = MaterialGroupNameList.Find(OldName);
    if (GroupIndex != INDEX_NONE) {     // Do contain OldName
        if (MaterialGroupNameList.Find(NewName) == INDEX_NONE) {    // Not contain NewName
            MaterialGroupNameList[GroupIndex] = NewName;
            return true;
        }
    }
    return false;
}

void USceneManagementAssetData::SyncActorByName()
{
    // gather all ALight actor in level
    TArray<AActor*> ActorList;
    TArray<FString> NameList;
    {
        UWorld* World = GEditor->GetEditorWorldContext().World();
        ULevel* Level = World->GetCurrentLevel();
        UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), ActorList);
        for (auto Actor : ActorList) {
            NameList.Add(Actor->GetName());
        }
    }

    ensure(LightingSolutionNameList.Num() == KeyLightParams.Num());
    ensure(LightingSolutionNameList.Num() == SceneAuxGroups.Num());
    ensure(LightingSolutionNameList.Num() == CharacterAuxGroups.Num());

    auto SyncActor = [ActorList, NameList](ULightParams* LightParams) {
        FString ActorName = LightParams->ActorName;
        AActor* LightActor = nullptr;
        do {
            if (ActorName.IsEmpty()) {
                break;
            }
            int FoundIndex = NameList.Find(ActorName);
            if (FoundIndex < 0) {
                break;
            }
            LightActor = ActorList[FoundIndex];
        } while (0);
        LightParams->LightActor = Cast<ALight>(LightActor);
    };

    // key light
    for (ULightParams *LightParams : KeyLightParams) {
        SyncActor(LightParams);
    }

    // Aux Light
    for (auto LightGroup : SceneAuxGroups) {
        for (ULightParams* LightParams : LightGroup->Array) {
            SyncActor(LightParams);
        }
    }
    for (auto LightGroup : CharacterAuxGroups) {
        for (ULightParams* LightParams : LightGroup->Array) {
            SyncActor(LightParams);
        }
    }
}

void USceneManagementAssetData::SyncDataByActor()
{
    for (ULightParams* LightParams : KeyLightParams) {
        LightParams->FromActor();
    }

    for (auto Group : SceneAuxGroups) {
        for (ULightParams* LightParams : Group->Array) {
            LightParams->FromActor();
        }
    }

    for (auto Group : CharacterAuxGroups) {
        for (ULightParams* LightParams : Group->Array) {
            LightParams->FromActor();
        }
    }
}

void USceneManagementAssetData::SyncMaterialByName()
{
}

void USceneManagementAssetData::SyncDataByMaterial(int SolutionIndex)
{
    if (SolutionIndex < 0) {
        return;
    }
    for (UMaterialInfo* MaterialInfo : MaterialSolutions[SolutionIndex]->SolutionItems) {
        MaterialInfo->FromMaterial();
    }
}

void USceneManagementAssetData::CleanUp()
{
    for (ULightParams* LightParams : KeyLightParams) {
        LightParams->LightActor = nullptr;
    }

    for (auto Group : SceneAuxGroups) {
        for (ULightParams* LightParams : Group->Array) {
            LightParams->LightActor = nullptr;
        }
    }

    for (auto Group : CharacterAuxGroups) {
        for (ULightParams* LightParams : Group->Array) {
            LightParams->LightActor = nullptr;
        }
    }
}

USceneManagementAssetData* USceneManagementAssetData::GetSelected(bool bAlertWhenEmpty)
{
    USceneManagementAssetData* AssetData = nullptr;
    do {
        SSettingsView *SettingView = SSettingsView::Get();
        if (!SettingView) {
            break;  // failed to get SettingView slate instance
        }

        AssetData = SettingView->AssetWrap->AssetData;
        if (!AssetData && bAlertWhenEmpty) {
            FText Title = FText::FromString("Warning");
            FText Content = FText::FromString(TEXT("Please select a USceneManagementAsset before edit!"));
            FMessageDialog::Open(EAppMsgType::Ok, Content, &Title);
        }

    } while (0);
    return AssetData;
}

USceneManagementAssetData* USceneManagementAssetData::GetEmpty()
{
    static USceneManagementAssetData* NullAsset = NewObject<USceneManagementAssetData>();
    if (!NullAsset->IsRooted()) {
        NullAsset->AddToRoot();
    }
    return NullAsset;
}

bool UEnabledGroupName::IsEmpty() const
{
    return GroupName.Num() == 0 ||
        (GroupName.Num() == 1 && GroupName.Contains(""));
}
