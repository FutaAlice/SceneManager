#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h" // USaveGame
#include "Engine/DataAsset.h"   // UDataAsset
#include "Materials/MaterialInstance.h" // UMaterialInstance
#include "SceneManagementAssetData.generated.h"

typedef int ELightCategory;

#define LightCategory_KeyLight          1
#define LightCategory_AuxLight          2
#define LightCategory_SceneLight        4
#define LightCategory_CharacterLight    8

typedef int EMPCOwner;

#define MPCOwner_ScenLight      1
#define MPCOwner_CharacterLight 2
#define MPCOwner_Wind           3
#define MPCOwner_PostProc       4

#define ERR_MAT_SOLUTION    -1
#define ERR_MAT_GROUP       -2
#define ERR_MAT_EXIST_INS   -3


UCLASS(hideCategories = Light)
class ULightParams : public UObject
{
    GENERATED_BODY()
public:
    ~ULightParams()
    {
        // TODO: FIX MEMORY LEAK
        // GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "SHIT");
    }

    void Clear();
    void FromActor();
    void ToActor();

    UPROPERTY(VisibleInstanceOnly, Category = "Light")
        FString ActorName;

    UPROPERTY(EditAnywhere, Transient, Category = "Light")
        class ALight *LightActor;

    UPROPERTY(EditAnywhere, Category = "LightParams")
        FRotator Rotation;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        float Intensity;
    UPROPERTY(EditAnywhere, Category = "LightParams")
        FLinearColor LightColor;
};

UCLASS()
class UGroupLightParams : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "LightParams")
        TArray<ULightParams*> Array;

    ULightParams* AddLightParam();
    void RemoveLightParam(ULightParams *LightParams);
};

UCLASS()
class UMaterialInfo : public UObject
{
    GENERATED_BODY()
public:
    template<typename OutType>
    OutType* GetMaterial()
    {
        if (!SoftObjectPath.IsAsset()) {
            return nullptr;
        }
        UObject* Object = SoftObjectPath.ResolveObject();
        if (!Object) {
            Object = SoftObjectPath.TryLoad();
        }
        return Cast<OutType>(Object);
    }

public:
    void ClearParams();
    void FromMaterial();
    void ToMaterial();

    // or MaterialInterface
    UPROPERTY(EditAnywhere, meta=(AllowedClasses="MaterialInstance"))
        FSoftObjectPath SoftObjectPath;

    //UPROPERTY(EditAnywhere)
    //    TMap<FString, float> ScalarParams;
    //UPROPERTY(EditAnywhere)
    //    TMap<FString, FLinearColor> VectorParams;

    UPROPERTY(BlueprintReadOnly)
        TMap<FString, float> ScalarParams;
    UPROPERTY(BlueprintReadOnly)
        TMap<FString, FLinearColor> VectorParams;
};

UCLASS()
class UGroupMaterialInfo : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Material")
        TArray<UMaterialInfo*> GroupItems;
};

UCLASS()
class USolutionMaterialInfo : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Material")
        TArray<UMaterialInfo*> SolutionItems;
};

UCLASS()
class UEnabledGroupName : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Material")
        TSet<FString> GroupName;

    bool IsEmpty() const;
    static UEnabledGroupName* Get();
    static bool Contains(FString Name);
};

/**
 * Scene Manager Asset Type
 */
UCLASS()
class USceneManagementAssetData : public UObject
{
    GENERATED_BODY()
public:
    void AddLightingSolution();
    void DuplicateLightingSolution(int SolutionIndex);
    void RemoveLightingSolution(int SolutionIndex);
    void RenameLightingSolution(int SolutionIndex, const FString& SolutionName);

    ULightParams* GetKeyLightParamsPtr(int SolutionIndex);
    UGroupLightParams* GetAuxLightGroupsPtr(int SolutionIndex, int LightCategory);

    bool GetGroupRange(FString GroupName, int &BeginIndex, int &EndIndex);
    void AddMaterialSolution();
    void AddMaterialGroup();
    int AddMaterial(FString GroupName, FSoftObjectPath DefaultValue = FSoftObjectPath());
    void DuplicateMaterialSolution(int SolutionIndex);
    void RemoveMaterialSolution(int SolutionIndex);
    void RemoveMaterialGroup(FString GroupName);
    void RenameMaterialSolution(int SolutionIndex, const FString& SolutionName);
    bool RenameMaterialGroup(FString OldName, FString NewName);
    
    void SyncActorByName();
    void SyncDataByActor();

    void SyncMaterialByName();
    void SyncDataByMaterial(int SolutionIndex);

    void CleanUp();

    static USceneManagementAssetData* GetSelected(bool bAlertWhenEmpty = true);
    static USceneManagementAssetData* GetEmpty();   // FOR DEBUG

public:
    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Lighting")
        TArray<FString> LightingSolutionNameList;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Lighting")
        TArray<ULightParams*> KeyLightParams;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Scene Lighting")
        TArray<UGroupLightParams*> SceneAuxGroups;

    UPROPERTY(VisibleAnywhere, Instanced, EditFixedSize, Category = "Character Lighting")
        TArray<UGroupLightParams*> CharacterAuxGroups;


    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Material")
        TArray<FString> MaterialSolutionNameList;

    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Material")
        TArray<FString> MaterialGroupNameList;

    UPROPERTY(EditAnywhere, EditFixedSize, Category = "Material")
        TArray<int> MaterialGroupIndexList; // group start index

    UPROPERTY(EditAnywhere, Instanced, EditFixedSize, Category = "Material")
        TArray<USolutionMaterialInfo*> MaterialSolutions;

    UPROPERTY(EditAnywhere, Instanced, Category = "Material")
        UEnabledGroupName* EnabledGroupName;
};
