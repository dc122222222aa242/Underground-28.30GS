#ifndef UTILS
#define UTILS
#include "../SDK/SDK.hpp"
#include <random>
#include <map>
#include <intrin.h>
#include <sstream>
using namespace SDK;
#include "Lootpool.h"
using namespace std;
typedef unsigned __int64 QWORD;
void* (__fastcall* GetInterfaceAddress)(UObject*, UClass*) = decltype(GetInterfaceAddress)(InSDKUtils::GetImageBase() + 0x118BD60);
inline UObject* (*StaticFindObject_)(UClass* Class, UObject* Package, const wchar_t* OrigInName, bool ExactClass) = decltype(StaticFindObject_)(__int64(GetModuleHandleW(0)) + 0x12933D0);
inline UObject* (*StaticLoadObject_)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation) = decltype(StaticLoadObject_)(InSDKUtils::GetImageBase() + 0x19EE1F4);
TArray<AActor*> Builds;
#pragma region FrameStructure
class FOutputDevice
{
public:
    bool bSuppressEventTag;
    bool bAutoEmitLineTerminator;
    uint8_t _Padding1[0x6];
};

struct FOutParmRec
{
    FProperty* Property;
    uint8* PropAddr;
    FOutParmRec* NextOutParm;
};


class FFrame : public FOutputDevice
{
public:
    void** VTable;
    UFunction* Node;
    UObject* Object;
    uint8* Code;
    uint8* Locals;
    FProperty* MostRecentProperty;
    uint8_t* MostRecentPropertyAddress;
    uint8* MostRecentPropertyContainer;
    TArray<void*> FlowStack;
    FFrame* PreviousFrame;
    FOutParmRec* OutParms;
    uint8_t _Padding1[0x20];
    FField* PropertyChainForCompiledIn;
    UFunction* CurrentNativeFunction;
    FFrame* PreviousTrackingFrame;
    bool bArrayContextFailed;

public:
    void StepCompiledIn(void* const Result, bool ForceExplicitProp = false) {
        if (Code && !ForceExplicitProp)
        {
            static void(__fastcall * FFrameStep) (FFrame*, UObject*, void* const) = decltype(FFrameStep) (InSDKUtils::GetImageBase() + 0x1402690);
            FFrameStep(this, Object, Result);
        }
        else
        {
            FField* _Prop = PropertyChainForCompiledIn;
            PropertyChainForCompiledIn = _Prop->Next;
            static void(__fastcall * FFrameStepExplicit)(FFrame*, void* const, FField*) = decltype(FFrameStepExplicit)(InSDKUtils::GetImageBase() + 0x103CFF4);
            FFrameStepExplicit(this, Result, _Prop);
        }
    }
    void* StepCompiledInRefInternal(void* _Tm)
    {
        MostRecentPropertyAddress = nullptr;
        MostRecentPropertyContainer = nullptr;

        if (Code)
        {
            static void(__fastcall * FFrameStep) (FFrame*, UObject*, void* const) = decltype(FFrameStep) (InSDKUtils::GetImageBase() + 0x1402690);
            FFrameStep(this, Object, _Tm);
        }
        else
        {
            FField* _Prop = PropertyChainForCompiledIn;
            PropertyChainForCompiledIn = _Prop->Next;
            static void(__fastcall * FFrameStepExplicit)(FFrame*, void* const, FField*) = decltype(FFrameStepExplicit)(InSDKUtils::GetImageBase() + 0x103CFF4);
            FFrameStepExplicit(this, _Tm, _Prop);
        }

        return MostRecentPropertyAddress ? MostRecentPropertyAddress : _Tm;
    }

    template <typename T>
    T& StepCompiledInRef()
    {
        T TempVal{};
        return *(T*)StepCompiledInRefInternal(&TempVal);
    }

    void IncrementCode() {
        Code = (uint8_t*)(__int64(Code) + (bool)Code);
    }
};
#pragma endregion
enum class ESpawnActorNameMode : uint8_t
{
    Required_Fatal,
    Required_ErrorAndReturnNull,
    Required_ReturnNull,
    Requested
};
struct FActorSpawnParameters
{
    FName Name = FName(0);
    UObject* Template = nullptr;
    UObject* Owner = nullptr;
    UObject* Instigator = nullptr;
    UObject* OverrideLevel = nullptr;
    UObject* OverrideParentComponent;
    ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
    ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
    uint8	bRemoteOwned : 1;
    uint8	bNoFail : 1;
    uint8	bDeferConstruction : 1;
    uint8	bAllowDuringConstructionScript : 1;
    uint8	bForceGloballyUniqueName : 1;
    ESpawnActorNameMode NameMode;
    EObjectFlags ObjectFlags;
    void* CallbackSum_Callable;
    void* CallbackSum_HeapAllocation;
};


typedef unsigned long DWORD;
template <typename T>
inline T* StaticFindObject(std::string ObjectPath, UClass* Class = UObject::FindClassFast("Object"), UObject* Package = nullptr)
{
    return (T*)StaticFindObject_(Class, Package, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
}

template <typename T>
inline T* StaticLoadObject(std::string Path, UClass* InClass = T::StaticClass(), UObject* Outer = nullptr)
{
    return (T*)StaticLoadObject_(InClass, Outer, std::wstring(Path.begin(), Path.end()).c_str(), nullptr, 0, nullptr, false);
}
bool bCreative = false;
bool bAllowUnsafeMoves = false;
bool bLateGame = false;
bool bRespawning = true; //for reload to work both on true
bool reloadMode = false; //for reload to work both on true
bool bTournament = true;
bool staticZones = false;

void ExecHook(std::string FuncPath, void* Detour, void** Original = nullptr) {
    auto Function = StaticFindObject<UFunction>(FuncPath, UFunction::StaticClass());
    if (Original) {
        *Original = (void*)Function->ExecFunction;
    }
    Function->ExecFunction = reinterpret_cast<UFunction::FNativeFuncPtr>(Detour);
}
UFortEngine* GetEngine()
{
    static auto Engine = (UFortEngine*)UEngine::GetEngine();
    return Engine;
}
UWorld* GetWorld()
{
    return UWorld::GetWorld();
}
UGameplayStatics* GetStatics()
{
    return (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;
}
FString Conv_NameToString(FName name)
{
    return ((UKismetStringLibrary*)UKismetStringLibrary::StaticClass()->DefaultObject)->Conv_NameToString(name);
}

FName Conv_StringToName(FString str)
{
    return ((UKismetStringLibrary*)UKismetStringLibrary::StaticClass()->DefaultObject)->Conv_StringToName(str);
}
AFortGameModeAthena* GetGameMode()
{
    return (AFortGameModeAthena*)GetWorld()->AuthorityGameMode;
}

AFortGameStateBR* GetGameState()
{
    if (!GetWorld()) {
        return 0;
    }
    return (AFortGameStateBR*)GetWorld()->GameState;
}

UKismetStringLibrary* GetKSL()
{
    auto Default = UKismetStringLibrary::GetDefaultObj();
    return Default;
}

UKismetSystemLibrary* GetKKSL()
{
    auto Default = UKismetSystemLibrary::GetDefaultObj();
    return Default;
}
UKismetMathLibrary* GetMath()
{
    return (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;
}

UFortGameStateComponent_BattleRoyaleGamePhaseLogic* GetGamePhaseLogic() {
    UFortGameStateComponent_BattleRoyaleGamePhaseLogic* (__fastcall * GetGamePhaseLogicOG)(UObject*) = decltype(GetGamePhaseLogicOG)(InSDKUtils::GetImageBase() + 0x1186F4C);
    return GetGamePhaseLogicOG(GetWorld());
}

template<class T>
inline T* Cast(UObject* Src)
{
    return Src && (Src->IsA(T::StaticClass())) ? (T*)Src : nullptr;
}
void VHook(void* Base, uintptr_t Index, void* Detour, void** Original = nullptr) {
    auto VTable = (*(void***)Base);
    if (!VTable)
        return;

    if (!VTable[Index])
        return;

    if (Original)
        *Original = VTable[Index];

    DWORD dwOld;
    VirtualProtect(&VTable[Index], 8, PAGE_EXECUTE_READWRITE, &dwOld);
    VTable[Index] = Detour;
    DWORD dwTemp;
    VirtualProtect(&VTable[Index], 8, dwOld, &dwTemp);
}

#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)
#define PI 					(3.1415926535897932f)

float RadiansToDegrees(float Radians)
{
    return Radians * (180.0f / PI);
}
inline void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
{
    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = (INV_PI * 0.5f) * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f * PI) * quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > HALF_PI)
    {
        y = PI - y;
        sign = -1.0f;
    }
    else if (y < -HALF_PI)
    {
        y = -PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    // 10-degree minimax approximation
    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ScalarCos = sign * p;
}

inline FVector RotatorToVector(FRotator Rotator)
{
    float CP, SP, CY, SY;
    SinCos(&SP, &CP, Rotator.pitch * (PI / 180.0f));
    SinCos(&SY, &CY, Rotator.Yaw * (PI / 180.0f));

    FVector NewV = FVector(CP * CY, CP * SY, SP);

    return NewV;
}
FRotator VectorToRotator(FVector DirectionVector)
{
    float Pitch = RadiansToDegrees(UKismetMathLibrary::asin(DirectionVector.Z));

    float Yaw = RadiansToDegrees(UKismetMathLibrary::Atan2(DirectionVector.Y, DirectionVector.X));

    return FRotator(Pitch, Yaw, 0.0f);
}
inline FRotator FQuatToRot(FQuat Quat)
{
    const float SingularityTest = Quat.Z * Quat.X - Quat.W * Quat.Y;
    const float YawY = 2.f * (Quat.W * Quat.Z + Quat.X * Quat.Y);
    const float YawX = (1.f - 2.f * (UKismetMathLibrary::Square(Quat.Y) + UKismetMathLibrary::Square(Quat.Z)));

    const float SINGULARITY_THRESHOLD = 0.4999995f;
    const float RAD_TO_DEG = (180.f) / PI;
    FRotator RotatorFromQuat;

    if (SingularityTest < -SINGULARITY_THRESHOLD)
    {
        RotatorFromQuat.pitch = -90.f;
        RotatorFromQuat.Yaw = UKismetMathLibrary::Atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = UKismetMathLibrary::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * UKismetMathLibrary::Atan2(Quat.X, Quat.W) * RAD_TO_DEG));
    }
    else if (SingularityTest > SINGULARITY_THRESHOLD)
    {
        RotatorFromQuat.pitch = 90.f;
        RotatorFromQuat.Yaw = UKismetMathLibrary::Atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = UKismetMathLibrary::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * UKismetMathLibrary::Atan2(Quat.X, Quat.W) * RAD_TO_DEG));
    }
    else
    {
        RotatorFromQuat.pitch = UKismetMathLibrary::asin(2.f * (SingularityTest)) * RAD_TO_DEG;
        RotatorFromQuat.Yaw = UKismetMathLibrary::Atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = UKismetMathLibrary::Atan2(-2.f * (Quat.W * Quat.X + Quat.Y * Quat.Z), (1.f - 2.f * (UKismetMathLibrary::Square(Quat.X) + UKismetMathLibrary::Square(Quat.Y)))) * RAD_TO_DEG;
    }

    return RotatorFromQuat;
}

FQuat FRotToQuat(FRotator Rot)
{
    const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
    const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    SinCos(&SP, &CP, Rot.pitch * DIVIDE_BY_2);
    SinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
    SinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}
AActor* SpawnActor(UClass* Class, FTransform& Transform, AActor* Owner = nullptr)
{
    FActorSpawnParameters addr{};

    addr.Owner = Owner;
    addr.bDeferConstruction = false;
    addr.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    return ((AActor * (*)(UWorld*, UClass*, FTransform const*, FActorSpawnParameters*))(InSDKUtils::GetImageBase() + 0x1999190))(UWorld::GetWorld(), Class, &Transform, &addr);
}

template<typename T>
inline T* SpawnActor(FVector Loc, FRotator Rot = FRotator(), AActor* Owner = nullptr, SDK::UClass* Class = T::StaticClass())
{
    FTransform Transform{};
    Transform.Scale3D = FVector{ 1,1,1 };
    Transform.Translation = Loc;
    Transform.Rotation = FRotToQuat(Rot);
    return (T*)SpawnActor(Class, Transform, Owner);
}
inline AActor* SpawnActor(FVector Location, FRotator Rotation, UClass* Class)
{
    FTransform Transform{};
    Transform.Rotation = FRotToQuat(Rotation);
    Transform.Translation = Location;
    Transform.Scale3D = FVector{ 1,1,1 };

    return SpawnActor(Class, Transform);
}
template<typename T>
T* SpawnActor(UClass* Class, FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{

    FTransform Transform{};
    Transform.Scale3D = FVector(1, 1, 1);
    Transform.Translation = Loc;
    Transform.Rotation = FRotToQuat(Rot);

    return (T*)SpawnActor(Class, Transform, Owner);
}


enum class EGetSparseClassDataMethod : uint8
{
    CreateIfNull,
    ArchetypeIfNull,
    ReturnIfNull,
};

template<class T>
T* GetSparseData(UObject* _this, EGetSparseClassDataMethod method = EGetSparseClassDataMethod::ArchetypeIfNull) {
    void* (__fastcall * GetSparseClassDataOG)(UObject * _this, EGetSparseClassDataMethod) = decltype(GetSparseClassDataOG)(InSDKUtils::GetImageBase() + 0x18A1CB8);
    return (T*)GetSparseClassDataOG(_this, method);
}
void(__fastcall* ApplyModSetToItemEntry)(UFortWeaponModSetData* ModSet, FFortItemEntry* entry) = decltype(ApplyModSetToItemEntry)(InSDKUtils::GetImageBase() + 0x9B34FC0);
inline float RandomFloatForMods(float AllWeightsSum)
{
    return (rand() * 0.000030518509f) * AllWeightsSum;
}
UFortWeaponModSetData* GetRandomBasedModset(UFortWeaponItemDefinition* Weapon) {
    auto static ModSetBucketDataTable = StaticLoadObject<UDataTable>("/WeaponMods/DataTables/ModSetBucketsBaseTable.ModSetBucketsBaseTable");

    if (!Weapon)
        return nullptr;

    if (!Weapon->bIsModifiableWeapon)
        return nullptr;

    FName ModTagName;

    for (auto Tag : Weapon->GameplayTags.GameplayTags) {
        if (Tag.TagName.ToString().contains("ModSetBucket")) {
            ModTagName = Tag.TagName;
        }
    }
    std::map<FName, FFortWeaponModSetRow*> OutMap;
    float TotalWeight = 0;
    for (TPair<FName, uint8_t*>& CurrentPair : ModSetBucketDataTable->RowMap)
    {
        if (((FFortWeaponModSetRow*)CurrentPair.Value())->BucketTag.TagName == ModTagName)
        {
            OutMap.insert_or_assign(CurrentPair.Key(), (FFortWeaponModSetRow*)CurrentPair.Value());
            TotalWeight += ((FFortWeaponModSetRow*)CurrentPair.Value())->RandomWeight;
        }
    }

    double accumulated = 0.0;
    float randomValue = RandomFloatForMods(TotalWeight);
    for (const auto& pair : OutMap) {
        accumulated += pair.second->RandomWeight;
        if (randomValue < accumulated) {
            return pair.second->Set;
        }
    }
    return nullptr;
}



AFortPickupAthena* SpawnPickup(FVector Loc, UFortItemDefinition* Def, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, int Count = 1, int LoadedAmmo = 0, AFortPawn* Pawn = nullptr)
{
    FTransform Transform{};
    Transform.Translation = Loc;
    Transform.Scale3D = FVector(1, 1, 1);
    AFortPickupAthena* Pickup = Cast<AFortPickupAthena>(UGameplayStatics::FinishSpawningActor(UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), AFortPickupAthena::StaticClass(), (FTransform&)Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr, ESpawnActorScaleMethod::SelectDefaultAtRuntime), (FTransform&)Transform, ESpawnActorScaleMethod::SelectDefaultAtRuntime));
    if (!Pickup)
        return nullptr;
    Pickup->bRandomRotation = true;
    if (Pawn)
        Pickup->PawnWhoDroppedPickup = Pawn;
    Pickup->PrimaryPickupItemEntry.ItemDefinition = Def;
    Pickup->PrimaryPickupItemEntry.Count = Count;
    Pickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
    Pickup->OnRep_PrimaryPickupItemEntry();

    Pickup->TossPickup(Loc, nullptr, -1, true, false, SourceTypeFlag, SpawnSource);

    // Set the pickup to auto-destroy after 30 seconds
    Pickup->SetLifeSpan(30.0f);

    if (SourceTypeFlag == EFortPickupSourceTypeFlag::Container)
    {
        Pickup->bTossedFromContainer = true;
        Pickup->OnRep_TossedFromContainer();
    }
    if (auto Weapon = Cast< UFortWeaponItemDefinition>(Def)) {
        auto modset = GetRandomBasedModset(Weapon);
        if (modset) {
            TArray<FFortModSetModEntry> Mods;
            switch (Weapon->Rarity) {
            case EFortRarity::Common:
                Mods = modset->CommonRarityMods;
                break;
            case EFortRarity::Uncommon:
                Mods = modset->UncommonRarityMods;
                break;
            case EFortRarity::Rare:
                Mods = modset->RareRarityMods;
                break;
            case EFortRarity::Epic:
                Mods = modset->EpicRarityMods;
                break;
            case EFortRarity::Legendary:
                Mods = modset->LengdaryRarityMods;
                break;
            default:
                break;
            }
            for (auto mod : Mods) {
                UFortWeaponModFunctionLibrary::ApplyWeaponModToPickup(Pickup, mod.ModDefinition);
            }
        }
    }


    return Pickup;
}

UField* GetProperty(UObject* _this, const std::string& ChildName, bool bWarnIfNotFound)
{
    for (auto CurrentClass = _this->Class; CurrentClass; CurrentClass = (UClass*)CurrentClass->Super)
    {
        UField* Property = CurrentClass->Children;

        if (Property)
        {

            std::string PropName = Property->Name.ToString();

            if (PropName == ChildName)
            {
                return Property;
            }

            while (Property)
            {
                if (PropName == ChildName)
                {
                    return Property;
                }

                Property = Property->Next;
                PropName = Property ? Property->Name.ToString() : "";
            }
        }
    }

    return nullptr;
}

int GetOffset(UObject* _this, const std::string& ChildName, bool bWarnIfNotFound)
{
    auto Property = GetProperty(_this, ChildName, bWarnIfNotFound);

    if (!Property)
        return -1;
    //0x3c
    return *(int*)(__int64(Property) + 0x3c);
}

float EvaluateScalableFloat(FScalableFloat& Float, float Level = 0)
{
    if (!Float.Curve.CurveTable)
        return Float.Value;

    float Out;
    UDataTableFunctionLibrary::EvaluateCurveTableRow(Float.Curve.CurveTable, Float.Curve.RowName, Level, nullptr, &Out, FString());
    return Out;
}

inline void SetScalableFloatVal(SDK::FScalableFloat& Float, float Value)
{
    Float.Value = Value;
    Float.Curve.CurveTable = nullptr;
    Float.Curve.RowName = SDK::FName();
}

FFortRangedWeaponStats* GetStats(UFortWeaponItemDefinition* Def)
{
    if (!Def)
    {
        std::cout << "!Def\n";
        return nullptr;
    }
    if (!Def->WeaponStatHandle.DataTable)
    {
        std::cout << "!Def->WeaponStatHandle.DataTable\n";
        return nullptr;
    }

    auto Val = Def->WeaponStatHandle.DataTable->RowMap.Search([Def](FName& Key, uint8_t* Value) {
        return Def->WeaponStatHandle.RowName == Key && Value;
        });

    return Val ? *(FFortRangedWeaponStats**)Val : nullptr;
}
FFortItemEntry* MakeItemEntry(UFortItemDefinition* ItemDefinition, int32 Count, int32 Level)
{
    FFortItemEntry* ItemEntry = new FFortItemEntry();

    ItemEntry->MostRecentArrayReplicationKey = -1;
    ItemEntry->ReplicationID = -1;
    ItemEntry->ReplicationKey = -1;

    ItemEntry->ItemDefinition = ItemDefinition;
    ItemEntry->Count = Count;
    ItemEntry->Durability = 1.f;
    ItemEntry->GameplayAbilitySpecHandle = FGameplayAbilitySpecHandle(-1);
    ItemEntry->ParentInventory.ObjectIndex = -1;
    ItemEntry->Level = Level;
    if (auto Weapon = ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()) ? ((UFortGadgetItemDefinition*)ItemDefinition)->GetWeaponItemDefinition() : Cast< UFortWeaponItemDefinition>(ItemDefinition))
    {
        auto Stats = GetStats(Weapon);
        ItemEntry->LoadedAmmo = Stats->ClipSize;
        if (Weapon->bUsesPhantomReserveAmmo)
            ItemEntry->PhantomReserveAmmo = Stats->InitialClips * Stats->ClipSize;
    }

    return ItemEntry;
}

void UpdateStormEffect(AFortPlayerController* PC) {
    static auto Effect = StaticLoadObject<UClass>("/Game/Athena/SafeZone/GE_OutsideSafeZoneDamage.GE_OutsideSafeZoneDamage_C");
    auto PlayerState = (AFortPlayerStateAthena*)(PC->PlayerState);

    bool Found = false;
    for (auto ActiveEffect : PlayerState->AbilitySystemComponent->ActiveGameplayEffects.GameplayEffects_Internal) {
        auto Handle = *(FActiveGameplayEffectHandle*)(__int64(&ActiveEffect) + 0x10);

        if (ActiveEffect.Spec.Def) {
            if (ActiveEffect.Spec.Def->IsA(Effect)) {
                FGameplayTag Tag = {};
                Tag.TagName = Conv_StringToName(L"SetByCaller.StormCampingDamage");

                PlayerState->AbilitySystemComponent->UpdateActiveGameplayEffectSetByCallerMagnitude(Handle, Tag, 1);
                Found = true;
                return;
            }
        }
    }
    if (!Found)
    {
        auto EffectHandle = FGameplayEffectContextHandle();
        auto SpecHandle = PlayerState->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(Effect, 0, EffectHandle);
        FGameplayTag Tag = {};
        Tag.TagName = Conv_StringToName(L"SetByCaller.StormCampingDamage");
        PlayerState->AbilitySystemComponent->UpdateActiveGameplayEffectSetByCallerMagnitude(SpecHandle, Tag, 1);
    }

}
static inline void ModifyInstructionLEA(uintptr_t instrAddr, uintptr_t targetAddr, int offset)
{
    int64_t delta = static_cast<int64_t>(targetAddr) -
        static_cast<int64_t>(instrAddr + offset + 4);

    auto patchLocation = reinterpret_cast<int32_t*>(instrAddr + offset);

    DWORD oldProtect;
    VirtualProtect(patchLocation, sizeof(int32_t), PAGE_EXECUTE_READWRITE, &oldProtect);

    *patchLocation = static_cast<int32_t>(delta);

    DWORD temp;
    VirtualProtect(patchLocation, sizeof(int32_t), oldProtect, &temp);
}

__forceinline static void ModifyInstruction(uintptr_t pinstrAddr, uintptr_t pDetourAddr)
{
    uint8_t* DetourAddr = (uint8_t*)pDetourAddr;
    uint8_t* instrAddr = (uint8_t*)pinstrAddr;
    int64_t delta = (int64_t)(DetourAddr - (instrAddr + 5));
    auto addr = reinterpret_cast<int32_t*>(instrAddr + 1);
    DWORD dwProtection;
    VirtualProtect(addr, 4, PAGE_EXECUTE_READWRITE, &dwProtection);

    *addr = static_cast<int32_t>(delta);

    DWORD dwTemp;
    VirtualProtect(addr, 4, dwProtection, &dwTemp);
}
struct Smth {
public:
    void* zSmth;
};
struct FNetResult {
public:
    uint64 Result = 0;
    TWeakObjectPtr<const UEnum> ResultEnumObj;
    const void* RawResultEnumObj = nullptr;
    FString ErrorContext;
    Smth NextResult;
};

void KickPlayer(AFortPlayerController* PC)
{
    FNetResult NetResult = FNetResult();
    void(__fastcall * CloseNetConnection)(UNetConnection*, FNetResult&&) = decltype(CloseNetConnection)(InSDKUtils::GetImageBase() + 0x446529C);
    NetResult.Result = 4;
    CloseNetConnection(PC->NetConnection, std::move(NetResult));
}

int GetRandomWeaponIndex(const std::vector<std::string>& vec) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(vec.size()) - 1);

    return dis(gen);
}

int RandomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);

    return distrib(gen);
}
bool RandomBool() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::bernoulli_distribution dist(0.5);

    return dist(gen);
}

void SetupWeaponAttachments(UFortWeaponRangedItemDefinition* W)
{
    std::vector<bool> activeSlots(4, false);
    int amountOfAttachments = 0;

    switch (W->Rarity) {
    case EFortRarity::Rare:
        amountOfAttachments = 2;
        break;
    case EFortRarity::Epic:
        amountOfAttachments = 3;
        break;
    case EFortRarity::Legendary:
        amountOfAttachments = 4;
        break;
    default:
        amountOfAttachments = 0;
        break;
    }

    std::vector<int> slotIndices = { 0, 1, 2, 3 };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(slotIndices.begin(), slotIndices.end(), gen);

    for (int i = 0; i < amountOfAttachments; i++) {
        activeSlots[slotIndices[i]] = true;
    }

    for (int i = 0; i < 4; i++) {
        std::cout << "Slot " << i << " active: " << (activeSlots[i] ? "true" : "false") << "\n";
    }

    for (int slotIdx = 0; slotIdx < 4; slotIdx++) {


        if (slotIdx >= Lootpool::Attachments.size()) continue;
        auto& Attachments = Lootpool::Attachments[slotIdx];
        if (Attachments.empty()) continue;

        int attIdx = RandomInt(0, Attachments.size() - 1);
        std::cout << Attachments[attIdx] << "\n";
        if (!activeSlots[slotIdx]) continue;
        UFortWeaponModItemDefinition* att = UObject::FindObject<UFortWeaponModItemDefinition>(Attachments[attIdx]);
        if (!att) {
            std::cout << Attachments[attIdx] << " is null\n";
            continue;
        }

        W->WeaponModSlots[slotIdx].WeaponMod = att;
    }
}
// Medallion Item Paths for Season
static const std::vector<std::string> MedallionPaths = {
    "FortniteGame/Plugins/GameFeatures/TakedownMedallion/Content/Items/Augments/BengalBasher/PAID_TDM_BengalBasher_Regen",
    "FortniteGame/Plugins/GameFeatures/TakedownMedallion/Content/Items/Augments/CeremonialGuard/PAID_TDM_CermonialGuard_Regen",
    "FortniteGame/Plugins/GameFeatures/TakedownMedallion/Content/Items/Augments/DiamondHeart/PAID_TDM_DiamondHeart_Regen",
    "FortniteGame/Plugins/GameFeatures/TakedownMedallion/Content/Items/Augments/MadameMoth/PAID_TDM_MadameMoth_Regen",
    "FortniteGame/Plugins/GameFeatures/TakedownMedallion/Content/Items/Augments/ZebraScramble/PAID_TDM_ZebraScramle_Regen"
};
#endif