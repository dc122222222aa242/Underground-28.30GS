#ifndef PHOEBE
#define PHOEBE
#pragma once
#include "Utils.h"
#include "../Hooking/HookUtils.h"
#include "../Hooking/MinHook/MinHook.h"

inline void (*sub_9D7780COG)(__int64) = nullptr;
void sub_9D7780C(__int64 ffgsfds) 
{
    UAthenaAIServicePlayerBots* AIServicePlayerBots = UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld());

    FTransform Transform{};

	AIServicePlayerBots->CachedAIPopulationTracker = (UAthenaAIPopulationTracker*)UGameplayStatics::SpawnObject(UAthenaAIPopulationTracker::StaticClass(), AIServicePlayerBots);
	if (!AIServicePlayerBots->CachedAIPopulationTracker)
        return sub_9D7780COG(ffgsfds);

    AIServicePlayerBots->CachedAIPopulationTracker->CachedGameMode = GetGameMode();
    AIServicePlayerBots->CachedAIPopulationTracker->AISystem = (UAISystem*)UWorld::GetWorld()->AISystem;

    return sub_9D7780COG(ffgsfds);
}

void InitializeMMRInfos()
{
    UAthenaAIServicePlayerBots* AIServicePlayerBots = UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld());
    AIServicePlayerBots->DefaultBotAISpawnerData = StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_AISpawnerData_Phoebe.BP_AISpawnerData_Phoebe_C");

	TArray<AActor*> VolumeS;
	UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPoiVolume::StaticClass(), &VolumeS);
	for (auto& Vol : VolumeS) {
		auto PoiVol = (AFortPoiVolume*)Vol;
		PoiVol->BrushComponent = (UBrushComponent*)UGameplayStatics::SpawnObject(UBrushComponent::StaticClass(), PoiVol);
		PoiVol->BrushComponent->K2_AttachToComponent(PoiVol->K2_GetRootComponent(), FName(0), EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
		AIServicePlayerBots->AddPOIVolume(PoiVol, EAthenaAIServicePOIList::MainBusDrop);
		FCachedPOIVolumeLocations newLoc{};
		newLoc.POIVolume = PoiVol;
		AIServicePlayerBots->CachedValidPOIVolumeLocations.Add(newLoc);
	}

    FMMRSpawningInfo NewSpawningInfo{};
    NewSpawningInfo.BotSpawningDataInfoTargetELO = 1000.f;
    NewSpawningInfo.BotSpawningDataInfoWeight = 100.f;
    NewSpawningInfo.NumBotsToSpawn = 0;
    NewSpawningInfo.AISpawnerData = AIServicePlayerBots->DefaultBotAISpawnerData;

    AIServicePlayerBots->DefaultAISpawnerDataComponentList = UFortAthenaAISpawnerData::CreateComponentListFromClass(AIServicePlayerBots->DefaultBotAISpawnerData, UWorld::GetWorld());
    AIServicePlayerBots->CachedMMRSpawningInfo.SpawningInfos.Add(NewSpawningInfo);
    AIServicePlayerBots->bWaitForNavmeshToBeLoaded = false;
    AIServicePlayerBots->GamePhaseToStartSpawning = EAthenaGamePhase::Warmup;
    *reinterpret_cast<bool*>(__int64(AIServicePlayerBots) + 0x820) = true; //bCanActivateBrain
   
}

void InitPhoebe() 
{
	HookUtils::Byte(InSDKUtils::GetImageBase() + 0x9D9EFF6, 0xeb); // match assignment check
    ModifyInstructionLEA(InSDKUtils::GetImageBase() + 0x9D9D171, InSDKUtils::GetImageBase() + 0xA17839C, 3);
    MH_CreateHook(LPVOID(__int64(InSDKUtils::GetImageBase()) + 0xA17839C), InitializeMMRInfos, nullptr);
	MH_CreateHook(LPVOID(__int64(InSDKUtils::GetImageBase()) + 0x9D7780C), sub_9D7780C, (LPVOID*)&sub_9D7780COG);
}

#endif 