#include <Windows.h>
#include <iostream>
#include <string>
#include "Hooking/MinHook/MinHook.h"
#include "SDK/SDK.hpp"
#include <thread>
#include "Server/GameMode.h"
#include "Server/Net.h"
#include "Server/Abilities.h"
#include "Server/PlayerController.h"
#include "Server/Pawn.h"
#include "Server/Damaging.h"
#include "Server/Ai.h"
#include "Server/Misc.h"
#include "Server/Phoebe.h"
#include <fstream>
#include "Server/MovementExtention.h"
using namespace SDK;
using namespace std;

void Dummy() { return; }
void Null(uintptr_t Address)
{
    MH_CreateHook((LPVOID)Address, Dummy, nullptr);
}

void (*CreateAndConfigureNavigationSystemOG)(UAthenaNavSystemConfig* This, UWorld* World);
void CreateAndConfigureNavigationSystem(UAthenaNavSystemConfig* This, UWorld* World)
{
    This->bPrioritizeNavigationAroundSpawners = true;
    This->bAutoSpawnMissingNavData = true;
    This->bAllowAutoRebuild = true;
    This->bSupportRuntimeNavmeshDisabling = false;
    This->bUsesStreamedInNavLevel = true;

    return CreateAndConfigureNavigationSystemOG(This, World);
}
double(__fastcall* OnCrashedOG)(__int64 a1);

double OnCrashedHK(__int64 a1) {
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(10)); 
        auto Message = Messages::HostRestarting;
        std::string HostType = "Reload";
        if (bLateGame)
            HostType = "LateGame";
        Replace(Message, "%host%", HostType);
        SendWebhook(Message);
        TerminateProcess(GetCurrentProcess(), 1);
        }).detach();
    return OnCrashedOG(a1);
}


DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* sFile;
    freopen_s(&sFile, "CONOUT$", "w", stdout);
    printf("Starting The GS!\n");
    MH_Initialize();
    
    srand(time(0));
    uintptr_t BaseAddr = InSDKUtils::GetImageBase();

	std::cout << BaseAddr << std:: endl;
    
    MH_CreateHook((LPVOID)(BaseAddr + Offsets::ProcessEvent), ProcessEventHook, (LPVOID*)&UObject::ProcessEventOG);
    MH_EnableHook((LPVOID)(BaseAddr + Offsets::ProcessEvent));

    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAthenaAIServiceBots VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAthenaBots VeryVerbose", nullptr);

    printf("Engine Started\n");
    LPVOID TickFlushAddr = (LPVOID)(BaseAddr + 0x19C6B78);
    
    LPVOID ReadyToStartMatchAddr = (LPVOID)(BaseAddr + 0x8491980);
    LPVOID OnCrashedAddr = (LPVOID)(BaseAddr + 0x5156D5C);
    LPVOID WorldNetModeAddr = (LPVOID)(BaseAddr + 0x118C5B0);
    LPVOID DriverNetModeAddr = (LPVOID)(BaseAddr + 0x118905C);
    LPVOID ChangeGameSessionIdAdd = (LPVOID)(BaseAddr + 0x38930E0);
    LPVOID NoMcpAddr = (LPVOID)(BaseAddr + 0x15A7EA8);
    LPVOID SpawnDefualtPawnAddr = (LPVOID)(BaseAddr + 0x8495904);
    LPVOID ServerAckAddr = (LPVOID)(BaseAddr + 0x663D450);
    LPVOID SetupInventoryAddr = (LPVOID)(BaseAddr + 0x838C4B4);
    LPVOID KickPlayerAddr = (LPVOID)(BaseAddr + 0x92CEF24);
    LPVOID KickPlayerAddr1 = (LPVOID)(BaseAddr + 0x92CEE18);
    LPVOID DispatchRequestAddr = (LPVOID)(BaseAddr + 0x73533A8);
    LPVOID GetPlayerViewPortAddr = (LPVOID)(BaseAddr + 0x118449C);
    LPVOID OnDamageServerAddr = (LPVOID)(BaseAddr + 0x8819168); 
    LPVOID ServerLoadingScreenAddr = (LPVOID)(BaseAddr + 0x3D66AD0);
    LPVOID IsValidAddr = (LPVOID)(BaseAddr + 0x506990C);
    LPVOID CreateSavedMoveAddr = (LPVOID)(BaseAddr + 0x629D6C4);
    LPVOID SpawnLootAddr = (LPVOID)(BaseAddr + 0x8820970);
    LPVOID ServerReadyToStartMatchAddr = (LPVOID)(BaseAddr + 0x967B124);
    LPVOID OnReloadAddr = (LPVOID)(BaseAddr + 0x9B278A4);
    LPVOID HandleMatchHasStartedAddr = (LPVOID)(BaseAddr + 0x848B01C);
    LPVOID ServerAckAddr1 = (LPVOID)(BaseAddr + 0x8538FE4);
    LPVOID GetMaxTickRateAddr = (LPVOID)(BaseAddr + 0x19C7824);
    LPVOID InternalActivateAbilityByTagAddr = (LPVOID)(BaseAddr + 0x725BBD4);
    LPVOID CanActivateAbilityAddr = (LPVOID)(BaseAddr + 0x720CD78);
    LPVOID TacticalSprintStarted = (LPVOID)(BaseAddr + 0x95A857C);
    LPVOID ServerOnExitVehicleAddr = (LPVOID)(BaseAddr + 0x95B2F38);
    LPVOID SetSafeZoneLocationsAddr = (LPVOID)(BaseAddr + 0x9B6E468);
    LPVOID ClientOnPawnDiedAddr = (LPVOID)(BaseAddr + 0x96BA59C);
	LPVOID OnProjectileHitAddr = (LPVOID)(BaseAddr + 0x2788BEC);
    LPVOID ServerNotifyPawnHitAddr = (LPVOID)(BaseAddr + 0x85CCF84);
    LPVOID OnCapsuleBeginOverlapAddr = (LPVOID)(BaseAddr + 0x2EA14E4);
    LPVOID OnPlayImpactFXAddr = (LPVOID)(BaseAddr + 0x1A7D920);
    LPVOID BrainStuffAddr = (LPVOID)(BaseAddr + 0x9D7780C);
    LPVOID SpawnRequestAddr = (LPVOID)(BaseAddr + 0x9D95A4C);
    LPVOID RegisterToWorldAddr = (LPVOID)(BaseAddr + 0xACACC64);
    LPVOID HasLevelsLoadedIDK = (LPVOID)(BaseAddr + 0x85226D4);
    LPVOID ServerUpdateLevelVisibilityAddr = (LPVOID)(BaseAddr + 0x6551438);
	LPVOID Wow = (LPVOID)(BaseAddr + 0x9D54AB8);
    LPVOID IdkMovement1 = (LPVOID)(BaseAddr + 0x1B009CC);
    LPVOID IdkMovement2 = (LPVOID)(BaseAddr + 0x1B00E28);
    LPVOID IdkMovement3 = (LPVOID)(BaseAddr + 0x921C738);
    LPVOID ActorGetWorldAddr = (LPVOID)(BaseAddr + 0x118AA50);
    LPVOID ConstructSlateBrushAddr = (LPVOID)(BaseAddr + 0x1A3479C);
    LPVOID GetTrackingModifierAddr = (LPVOID)(BaseAddr + 0x9CE2278);
    LPVOID OnPossesedPawnDiedAddr = (LPVOID)(BaseAddr + 0x9C10A6C);
    LPVOID CreateNavMeshAddr = (LPVOID)(BaseAddr + 0x1EF7F9C);
    LPVOID StartPhysUpdateAddr = (LPVOID)(BaseAddr + 0x1FFD5F0);
    LPVOID MMEGenerateNameAddr = (LPVOID)(BaseAddr + 0x1B00E28);
    Null(BaseAddr + 0x38930E0);
    Null(BaseAddr + 0x22EFFD0);
    Null(BaseAddr + 0x22F0148);
    Null(BaseAddr + 0x1296E4C);
    Null(BaseAddr + 0x12967D0);
    Null(BaseAddr + 0x9677DAC);
    Null(BaseAddr + 0x43DA840);
    Null(BaseAddr + 0x9F74BAC);
    MH_CreateHook(MMEGenerateNameAddr, sub_1B00E28, (LPVOID*)&sub_1B00E28OG);
    MH_CreateHook(StartPhysUpdateAddr, StartUpdatePhys, (LPVOID*)&StartUpdatePhysOG);
    MH_CreateHook(OnCrashedAddr, OnCrashedHK, (LPVOID*)&OnCrashedOG);
    MH_CreateHook(OnPossesedPawnDiedAddr, OnPossesedPawnDiedHK, (LPVOID*)&OnPossesedPawnDiedOG);
    MH_CreateHook(GetTrackingModifierAddr, GetTrackingModifierInternalHK, (LPVOID*)&GetTrackingModifierInternalOG);
    MH_CreateHook(KickPlayerAddr, KickPlayerHK, nullptr);
    MH_CreateHook(IsValidAddr, sub_506990C, (LPVOID*)&IsValid);
    MH_CreateHook(KickPlayerAddr1, KickPlayerHK, nullptr);
    MH_CreateHook(HasLevelsLoadedIDK, ReturnTrue, nullptr);
    MH_CreateHook(WorldNetModeAddr, GetNetMode, nullptr);
    MH_CreateHook(DriverNetModeAddr, GetNetMode, nullptr);
	MH_CreateHook(TickFlushAddr, TickFlushHook, (LPVOID*)&TickFlush);
    MH_CreateHook(SpawnDefualtPawnAddr, SpawnDefaultPawnHK, nullptr);
    MH_CreateHook(ReadyToStartMatchAddr, ReadyToStartMatchHook, (LPVOID*)&ReadyToStartMatchOG);
    MH_CreateHook(ServerAckAddr, ServerAcknowledgePossessionHook, nullptr);
    MH_CreateHook(NoMcpAddr, NoMcp, nullptr);
    MH_CreateHook(DispatchRequestAddr, MCP_DispatchRequest, (LPVOID*)&MCP_DispatchRequestOG);
    MH_CreateHook(GetPlayerViewPortAddr, GetPlayerViewPointHook, (LPVOID*)&GetPlayerViewPoint);
    MH_CreateHook(OnDamageServerAddr, OnDamageServerHook, (LPVOID*)&OnDamageServer);
    MH_CreateHook(SetupInventoryAddr, SetupInventory, (LPVOID*)&SetupInventoryOG);
    MH_CreateHook(ServerLoadingScreenAddr, ServerLoadingScreenDropped, (LPVOID*)&ServerLoadingScreenDroppedOG);
    MH_CreateHook(SpawnLootAddr, SpawnLoot, nullptr);
    MH_CreateHook(ServerReadyToStartMatchAddr, ServerReadyToStartMatch, (LPVOID*)&ServerReadyToStartMatchOG);
    MH_CreateHook(OnReloadAddr, OnReload, (LPVOID*)&OnReloadOG);
    MH_CreateHook(HandleMatchHasStartedAddr, HandleStartMatchHK, (LPVOID*)&HandleStartMatchOG);
    MH_CreateHook(ServerAckAddr1, ServerAcknowledgePossessionHook, nullptr);
    MH_CreateHook(GetMaxTickRateAddr, GetMaxTickRate, nullptr);
    MH_CreateHook(CanActivateAbilityAddr, CanActivateAbility, nullptr);
    MH_CreateHook(TacticalSprintStarted, OnTacticalSprintStartedHK, (LPVOID*)&OnTacticalSprintStartedOG);
    MH_CreateHook(ClientOnPawnDiedAddr, ClientOnPawnDied, (LPVOID*)&ClientOnPawnDiedOG);
	MH_CreateHook(OnProjectileHitAddr, ProjectileRequestHook, (LPVOID*)&ProjectileRequest);
    MH_CreateHook(ServerNotifyPawnHitAddr, ServerNotifyPawnHitHK, (LPVOID*)&ServerNotifyPawnHitOG);
    MH_CreateHook(OnCapsuleBeginOverlapAddr, OnCapsuleBeginOverlap, (LPVOID*)&OnCapsuleBeginOverlapOG);
    MH_CreateHook(Wow, AIBotInventoryOnSpawned, (void**)&AIBotInventoryOnSpawnedOG);
    MH_CreateHook(CreateNavMeshAddr, CreateAndConfigureNavigationSystem, (LPVOID*)&CreateAndConfigureNavigationSystemOG);
    InitPhoebe();
    MH_EnableHook(MH_ALL_HOOKS);
    for (int i = 0; i < UObject::GObjects->Num(); i++)
    {
        auto Object = UObject::GObjects->GetByIndex(i);

        if (!Object)
            continue;

        if (Object->IsA(UAbilitySystemComponent::StaticClass()))
        {
            VHook(Object, 0x118, InternalServerTryActiveAbilityHook, nullptr);
        }
    }
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x22A, ServerExecuteInventoryItemHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x24A, ServerCreateBuildingActorHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x251, ServerBeginEditingBuildingActorHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x24C, ServerEditBuildingActorHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x24F, ServerEndEditingBuildingActorHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x1F4, ServerCheat);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x1F6, ServerPlayEmoteItemHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x235, ServerAttemptInventoryDrop);
    VHook(APlayerPawn_Athena_C::GetDefaultObj(), 0x241, ServerHandlePickupInfo);
    VHook(APlayerPawn_Athena_C::GetDefaultObj(), 0x250, ServerSendZiplineState);
    VHook(APlayerPawn_Athena_C::GetDefaultObj(), 0x13D, NetMulticastDamageCues, (void**)&NetMulticastDamageCuesOG);
	VHook(UFortControllerComponent_Aircraft::GetDefaultObj(), 0xa4, ServerAttemptAircraftJumpHook);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x27A, ServerReturnToMainMenu);
    VHook(AFortPlayerControllerAthena::GetDefaultObj(), 0x575, ClientIsReadyToRespawn, (void**)&ClientIsReadyToRespawnOG);
    VHook(UFortAthenaAISpawnerDataComponent_AIBotBehavior::GetDefaultObj(), 0x59, AIBotBehaviorOnSpawned, (void**)&AIBotBehaviorOnSpawnedOG);
    VHook(UFortAthenaAISpawnerDataComponent_AIBotGameplayAbilityBase::GetDefaultObj(), 0x59, AIBotGameplayAbilityBaseOnSpawned, (void**)&AIBotGameplayAbilityBaseOnSpawnedOG);
    VHook(UFortAthenaAISpawnerDataComponent_AIBotSkillset::GetDefaultObj(), 0x59, AIBotSkillsetOnSpawned, (void**)&AIBotSkillsetOnSpawnedOG);
    VHook(UFortAthenaAISpawnerDataComponent_CosmeticLoadout::GetDefaultObj(), 0x59, AIBotCosmeticOnSpawned, (void**)&AIBotCosmeticOnSpawnedOG);
    VHook(AAthenaNavMesh::GetDefaultObj(), 0x21, ReturnTrue, nullptr);
    ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_SpawnPickupInWorld", K2_SpawnPickupInWorldHook);
    ExecHook("/Script/FortniteGame.FortKismetLibrary.SpawnItemVariantPickupInWorld", SpawnItemVariantPickupInWorldHook);
    ExecHook("/Script/FortniteGame.FortKismetLibrary.PickLootDrops", PickLootDropsHook);
    ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_SpawnPickupInWorldWithClass", K2_SpawnPickupInWorldWithClassHook);
    ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_SpawnPickupInWorldWithClassAndItemEntry", K2_SpawnPickupInWorldWithClassAndItemEntry);
    ExecHook("/Script/FortniteGame.FortPawn.MovingEmoteStopped", MovingEmoteStopped);
    //ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_RemoveFortItemFromPlayer", K2_RemoveFortItemFromPlayer, (void**)&K2_RemoveFortItemFromPlayerOG);
    //ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayer", K2_RemoveItemFromPlayer, (void**)&K2_RemoveItemFromPlayerOG);
    //ExecHook("/Script/FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayerByGuid", K2_RemoveItemFromPlayerByGuid, (void**)&K2_RemoveItemFromPlayerByGuidOG);

    *(bool*)(BaseAddr + 0x1164007B) = false; //GIsClient
    *(bool*)(BaseAddr + 0x1164000D) = true; //GIsServer
    //*(bool*)(InSDKUtils::GetImageBase() + 0x117E1128) = true; //should be set automaticly in Backend ngl
    //*(int*)(InSDKUtils::GetImageBase() + 0x117B60D0) = 7; //manually setting log level for movement ext, i guess this s VeryVerbose or smth
    *(int*)(InSDKUtils::GetImageBase() + 0x117B5EC0) = 5;
    //*(int*)(InSDKUtils::GetImageBase() + 0x117BE960) = 7;//enable all liwing world manage logs
    //*(int*)(BaseAddr + 0x118EBBA8) = 7;
    //*(int*)(BaseAddr + 0x117B5FD8) = 7;
    
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), TEXT("log LogFortUIDirector off"), nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), TEXT("log LogNet off"), nullptr);

    UFortEngine::GetEngine()->GameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), bCreative ? L"open Creative_NoApollo_Terrain" : L"open Helios_Terrain", nullptr);
    SetConsoleTitleA("28.30 GameServer | Initializing");
    return 0;
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        FreeLibraryAndExitThread(hModule, 0);
        break;
    }
    return TRUE;
}
