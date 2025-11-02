#ifndef GAMEMODE
#define GAMEMODE
#pragma once
#include "Net.h"
#include "Abilities.h"
#include "Inventory.h"
#include "DiscordIntegration.h"
#include "Vehicles.h"
#include "GamePhases.h"
#include "Ai.h"

void(__fastcall* HandleStartMatchOG)(AFortGameModeAthena* GameMode);
void HandleStartMatchHK(AFortGameModeAthena* GameMode) {
    HandleStartMatchOG(GameMode);
    auto GameState = (AFortGameStateBR*)(GameMode->GameState);

    if (!bCreative)
    {
        auto GamePhaseLogic = GetGamePhaseLogic();
        if (GamePhaseLogic) {
            SetGamePhase(GamePhaseLogic, EAthenaGamePhase::Warmup);
        }
    }
}

void FixShotguns() {
    auto Shells = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    auto HeavyAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");;
    for (size_t i = 0; i < UObject::GObjects->Num(); i++)
    {
        auto Obj = UObject::GObjects->GetByIndex(i);
        if (!Obj || Obj->IsDefaultObject() || !Obj->IsA(UFortWeaponRangedItemDefinition::StaticClass()))
            continue;
        auto Def = (UFortWeaponRangedItemDefinition*)Obj;
        auto AmmoData = Def->GetAmmoWorldItemDefinition_BP();
        if (AmmoData && AmmoData != Def && (AmmoData == Shells || AmmoData == HeavyAmmo))
        {
            if (!Def->WeaponStatHandle.DataTable || !Def->WeaponStatHandle.RowName.ComparisonIndex)
                return;

            auto DataTable = Def->WeaponStatHandle.DataTable;
            FName Name = Def->WeaponStatHandle.RowName;

            TMap<FName, FFortRangedWeaponStats*>& WeaponStatTable = *(TMap<FName, FFortRangedWeaponStats*>*)(int64(DataTable) + 0x30);

            for (auto& Pair : WeaponStatTable)
            {
                if (Pair.Key().ComparisonIndex == Name.ComparisonIndex)
                {
                    Pair.Value()->KnockbackMagnitude = 0;
                    Pair.Value()->KnockbackZAngle = 0;
                    Pair.Value()->LongRangeKnockbackMagnitude = 0;
                    Pair.Value()->MidRangeKnockbackMagnitude = 0;
                    break;
                }
            }
        }
    }
}

void SpawnFloorLoot()
{
    if (bLateGame)
        return;
    static bool Spawned = false;
    if (!Spawned) {
        TArray<AActor*> FloorLootSpawnersWarmup;
        UClass* SpawnerClass = StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
        GetStatics()->GetAllActorsOfClass(GetWorld(), SpawnerClass, &FloorLootSpawnersWarmup);

        TArray<AActor*> FloorLootSpawners;
        SpawnerClass = StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
        GetStatics()->GetAllActorsOfClass(GetWorld(), SpawnerClass, &FloorLootSpawners);

        if (FloorLootSpawners.Num() + FloorLootSpawnersWarmup.Num() >= 1000)
        {
            for (size_t i = 0; i < FloorLootSpawnersWarmup.Num(); i++)
            {
                FloorLootSpawnersWarmup[i]->K2_DestroyActor();
            }

            for (size_t i = 0; i < FloorLootSpawners.Num(); i++)
            {
                FloorLootSpawners[i]->K2_DestroyActor();
            }
            Spawned = true;
        }
        FloorLootSpawnersWarmup.Free();
        FloorLootSpawners.Free();
    }

}


bool (*ReadyToStartMatchOG)(AFortGameModeAthena*);
bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
    auto result = ReadyToStartMatchOG(GameMode);
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), bCreative ? AFortPlayerStartCreative::StaticClass() : AFortPlayerStartWarmup::StaticClass(), &PlayerStarts);
    int Spots = PlayerStarts.Num();

    PlayerStarts.Clear();
    if (Spots == 0 && !bCreative)
        return false;

    auto GameState = (AFortGameStateBR*)(GameMode->GameState);
    static bool InitPlaylist = false;

    if (!InitPlaylist)
    {
        InitPlaylist = true;

        auto Playlist = bCreative ?
            StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2") :
            (bTournament ?
                StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Showdown/Tournament/RankedCups/Playlist_ShowdownTournament_RC_Solo.Playlist_ShowdownTournament_RC_Solo") :
                StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo")
                );

        if (bRespawning) {
            Playlist->bRespawnInAir = true;
            Playlist->bForceRespawnLocationInsideOfVolume = true;
            SetScalableFloatVal(Playlist->RespawnHeight, 15000);
            SetScalableFloatVal(Playlist->RespawnTime, 5);
            Playlist->RespawnType = EAthenaRespawnType::InfiniteRespawn;
        }

        GameState->bIsUsingDownloadOnDemand = false;
        GameState->OnRep_IsUsingDownloadOnDemand();

        GameMode->WarmupRequiredPlayerCount = 1;
        GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;

        GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
        GameState->CurrentPlaylistInfo.MarkArrayDirty();
        GameState->CurrentPlaylistId = Playlist->PlaylistId;
        GameMode->CurrentPlaylistId = Playlist->PlaylistId;
        GameMode->CurrentPlaylistName = Playlist->PlaylistName;

        GameMode->GameSession->MaxPlayers = 100;
        GameMode->PlaylistHotfixOriginalGCFrequency = Playlist->GarbageCollectionFrequency;

        UAthenaAISettings* AISettingsInstance = nullptr;
        for (int i = 0; i < UObject::GObjects->Num(); i++)
        {
            auto Object = UObject::GObjects->GetByIndex(i);
            if (Object && Object->Class == UAthenaAISettings::StaticClass())
            {
                AISettingsInstance = (UAthenaAISettings*)Object;
                break;
            }
        }

        if (AISettingsInstance) {
            GameMode->AISettings = AISettingsInstance;

            GameMode->AISettings->AIServices.Free();
            GameMode->AISettings->AIServices.Add(StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/AIServices/BP_Phoebe_AIService_Loot.BP_Phoebe_AIService_Loot_C"));
            GameMode->AISettings->AIServices.Add(UAthenaAIServicePlayerBots::StaticClass());
            GameMode->AISettings->AIServices.Add(UAthenaAIServiceVehicle::StaticClass());
            GameMode->AISettings->AIServices.Add(UAthenaAIServiceGroup::StaticClass());
            GameMode->AISettings->AIServices.Add(UAthenaAIServiceZoneManager::StaticClass());
            GameMode->AISettings->AIServices.Add(StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/AIServices/BP_Phoebe_AIService_Cover.BP_Phoebe_AIService_Cover_C"));

            GameMode->AISettings->MaxFootstepHearingRange = 3000.0f;
            GameMode->AISettings->ReducedDeAggroRange = 3500.0f;
        }
        else {
            GameMode->AISettings = Playlist->AISettings.Get();
        }

        UFortGameStateComponent_BattleRoyaleGamePhaseLogic* GamePhaseLogic = (UFortGameStateComponent_BattleRoyaleGamePhaseLogic*)GameState->GetComponentByClass(UFortGameStateComponent_BattleRoyaleGamePhaseLogic::StaticClass());
        GamePhaseLogic->AirCraftBehavior = Playlist->AirCraftBehavior;
        if (!bLateGame)
            GameState->DefaultParachuteDeployTraceForGroundDistance = 10000.0f;
        else
            GameState->DefaultParachuteDeployTraceForGroundDistance = 100.0f;

    }
    if (!GameState->MapInfo)
        return false;

    if (!bIsListening) {

        GetWorld()->ServerStreamingLevelsVisibility = SpawnActor<AServerStreamingLevelsVisibility>(AServerStreamingLevelsVisibility::StaticClass(), {}, {});
        auto GamePhaseLogic = GetGamePhaseLogic();
        for (auto& AdditionLevel : GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevels)
        {
            FAdditionalLevelStreamed NewLevel{};
            NewLevel.bIsServerOnly = false;
            auto NewStr = AdditionLevel.ObjectID.GetAssetPathName();
            NewLevel.LevelName = Conv_StringToName(std::wstring(NewStr.begin(), NewStr.end()).c_str());
            bool bOutSuccess = false;
            ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), AdditionLevel, {}, {}, &bOutSuccess, FString(), nullptr, false);
            GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
        }

        for (auto& AdditionLevel : GameState->CurrentPlaylistInfo.BasePlaylist->AdditionalLevelsServerOnly)
        {
            FAdditionalLevelStreamed NewLevel{};
            NewLevel.bIsServerOnly = true;
            auto NewStr = AdditionLevel.ObjectID.GetAssetPathName();
            NewLevel.LevelName = Conv_StringToName(std::wstring(NewStr.begin(), NewStr.end()).c_str());
            bool bOutSuccess = false;

            ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), AdditionLevel, {}, {}, &bOutSuccess, FString(), nullptr, false);
            GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
        }

        GameState->OnRep_AdditionalPlaylistLevelsStreamed();
        GameState->OnFinishedStreamingAdditionalPlaylistLevel();
        GameState->OnRep_CurrentPlaylistId();
        GameState->OnRep_CurrentPlaylistInfo();

        GameMode->bDisableGCOnServerDuringMatch = true;
        GameMode->bPlaylistHotfixChangedGCDisabling = true;

        GameMode->DefaultPawnClass = APlayerPawn_Athena_C::StaticClass();
        Listen();
        auto Message = Messages::HostStarted;
        std::string HostType = "Reload";
        if (bLateGame)
            HostType = "LateGame";
        Replace(Message, "%host%", HostType);
        SendWebhook(Message);
        GameMode->bWorldIsReady = true;
        if (!bCreative) {
            auto mutatorDeimosSurvival = SpawnActor<AFortAthenaMutator_DeimosSurvival>({});
            auto mutatorSafeZone = SpawnActor< AFortAthenaMutator_SafeZoneStartupHelper>({});
            GameMode->AddMutatorToList(mutatorDeimosSurvival);
            GameMode->AddMutatorToList(mutatorSafeZone);
            mutatorSafeZone->OverrideSafeZoneType = GetGameState()->CurrentPlaylistInfo.BasePlaylist->SafeZoneStartUp;
            mutatorDeimosSurvival->OnPlaylistDataLoaded(GameMode->CurrentPlaylistName, GameState->CurrentPlaylistInfo.BasePlaylist->GameplayTagContainer);

        }
        std::string Title = "28.30 Gameserver | Listening";

        SetConsoleTitleA(Title.c_str());
    }
    if (!bCreative)
    {
        SpawnCars();
        SpawnFloorLoot();
    }


    if (GetGameState()->PlayersLeft > 0)
    {
        return true;
    }
    else
    {
        auto GamePhaseLogic = GetGamePhaseLogic();
        if (GamePhaseLogic) {
            float TimeSeconds = GetStatics()->GetTimeSeconds(GetWorld());
            GamePhaseLogic->WarmupCountdownEndTime = TimeSeconds + 60;
            GamePhaseLogic->WarmupCountdownStartTime = TimeSeconds;
            GamePhaseLogic->WarmupEarlyCountdownDuration = GamePhaseLogic->WarmupCountdownDuration;
            GamePhaseLogic->OnRep_WarmupCountdownEndTime();
        }

    }
    return false;
}

APawn* SpawnDefaultPawnHK(AFortGameModeBR* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* Spot) {

    auto Pawn = (AFortPlayerPawnAthena*)GameMode->SpawnDefaultPawnAtTransform(NewPlayer, Spot->GetTransform());
    if (!NewPlayer->MatchReport)
    {
        NewPlayer->MatchReport = (UAthenaPlayerMatchReport*)UGameplayStatics::SpawnObject(UAthenaPlayerMatchReport::StaticClass(), NewPlayer);
    }
    static bool IsFirstPlayer = false;
    if (!IsFirstPlayer)
    {
        FixShotguns();

        IsFirstPlayer = true;

    }
    return Pawn;
}


#endif // !GAMEMODE
