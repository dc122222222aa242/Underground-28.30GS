#ifndef PLAYERCONTROLLER
#define PLAYERCONTROLLER
#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "Looting.h"
#include "Vehicles.h"
#include "GamePhases.h"
#include "Abilities.h"
#include "SpecialActor.h"
#include "MedallionRegen.h"
#include <fstream>

inline void SetupLateGameItems(AFortPlayerControllerAthena* PC) {

	/*static auto LightAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
			static auto MediumAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
			static auto HeavyAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
			static auto Shells = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");

			static auto Wood = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
			static auto Stone = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
			static auto Metal = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

			static auto Pump = UObject::FindObject<UFortWeaponRangedItemDefinition>("FortWeaponRangedItemDefinition WID_Shotgun_Pump_Paprika_Athena_R.WID_Shotgun_Pump_Paprika_Athena_R");
			static auto Ar = StaticLoadObject<UFortWeaponRangedItemDefinition>("/PaprikaCoreWeapons/Items/Weapons/PaprikaAR_DPS/WID_Assault_Paprika_DPS_Athena_UR_Boss.WID_Assault_Paprika_DPS_Athena_UR_Boss");
			static auto Sniper = StaticLoadObject<UFortWeaponRangedItemDefinition>("/PaprikaCoreWeapons/Items/Weapons/PaprikaSniper/WID_Sniper_Paprika_Athena_SR.WID_Sniper_Paprika_Athena_SR");

			static auto Shield = StaticLoadObject<UFortItemDefinition>("/PaprikaConsumables/Gameplay/ShieldRefresh/WID_Paprika_ShieldPot.WID_Paprika_ShieldPot");

			static auto GrappleBlade = StaticLoadObject<UFortItemDefinition>("/GrappleWeapon/Items/GrappleKnife/V2/WID_GrappleKnife_VR.WID_GrappleKnife_VR");

			GiveItem(PlayerController, Shield);
			GiveItem(PlayerController, GrappleBlade, 1, 999);

			GiveItem(PlayerController, Ar, 1, GetStats(Ar)->ClipSize);
			GiveItem(PlayerController, Pump, 1, GetStats(Pump)->ClipSize);
			GiveItem(PlayerController, Sniper, 1, GetStats(Sniper)->ClipSize);

			GiveItem(PlayerController, Wood, 500);
			GiveItem(PlayerController, Stone, 500);
			GiveItem(PlayerController, Metal, 500);

			GiveItem(PlayerController, LightAmmo, 500);
			GiveItem(PlayerController, MediumAmmo, 500);
			GiveItem(PlayerController, HeavyAmmo, 50);
			GiveItem(PlayerController, Shells, 250);*/



	std::thread([PC = PC]() {
		static auto LightAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
		static auto MediumAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
		static auto HeavyAmmo = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
		static auto Shells = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");

		static auto Wood = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static auto Stone = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static auto Metal = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
		static auto Shield = StaticLoadObject<UFortItemDefinition>("/PaprikaConsumables/Gameplay/ShieldRefresh/WID_Paprika_ShieldPot.WID_Paprika_ShieldPot");
		static auto Shield2 = StaticLoadObject<UFortItemDefinition>("/PaprikaConsumables/Gameplay/ShieldSmallRefresh/WID_Paprika_ShieldSmall.WID_Paprika_ShieldSmall");

		GiveItem(PC, Shield, 3);
		GiveItem(PC, Shield2, 6);
		GiveItem(PC, Wood, 500);
		GiveItem(PC, Stone, 500);
		GiveItem(PC, Metal, 500);

		GiveItem(PC, LightAmmo, 250);
		GiveItem(PC, MediumAmmo, 150);
		GiveItem(PC, HeavyAmmo, 15);
		GiveItem(PC, Shells, 30);
		}).detach();

	std::thread([PC = PC]() {
		/*

		WEAPON MODS

		UFortWeaponModItemDefinition* att1 = UObject::FindObject<UFortWeaponModItemDefinition>("FortWeaponModItemDefinitionOptic WMOID_Optic_Holo.WMOID_Optic_Holo");
		FirstSlotItemDef->WeaponModSlots[0].WeaponMod = att1;*/

		auto Name = Lootpool::Weapons::First[RandomInt(0, Lootpool::Weapons::First.size() - 1)];
		auto ItemDef = UObject::FindObject<UFortWeaponRangedItemDefinition>(Name);
		//SetupWeaponAttachments(ItemDef);
		if (ItemDef) {
			GiveItem(PC, ItemDef, 1, GetStats(ItemDef)->ClipSize);
		}
		}).detach();

	std::thread([PC = PC]() {
		auto Name = Lootpool::Weapons::Second[RandomInt(0, Lootpool::Weapons::Second.size() - 1)];
		auto ItemDef = UObject::FindObject<UFortWeaponRangedItemDefinition>(Name);
		//SetupWeaponAttachments(ItemDef);
		if (ItemDef) {
			GiveItem(PC, ItemDef, 1, GetStats(ItemDef)->ClipSize);
		}
		}).detach();

	std::thread([PC = PC]() {
		if (RandomBool()) {
			auto ItemDef = UObject::FindObject<UFortItemDefinition>("FortWeaponMeleeItemDefinition WID_GrappleKnife_VR.WID_GrappleKnife_VR");
			GiveItem(PC, ItemDef, 1, 4);
		}
		else {
			auto Name = Lootpool::Weapons::Third[RandomInt(0, Lootpool::Weapons::Third.size() - 1)];
			auto ItemDef = UObject::FindObject<UFortWeaponRangedItemDefinition>(Name);
			//SetupWeaponAttachments(ItemDef);
			GiveItem(PC, ItemDef, ItemDef->MaxStackSize.Value, GetStats(ItemDef)->ClipSize);
		}
		}).detach();

}

ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor*, __int64, UClass*, int, int, uint8_t, AFortPlayerController*) = decltype(ReplaceBuildingActor)(InSDKUtils::GetImageBase() + 0x889BD24);
void (*RemoveFromAlivePlayers)(AFortGameModeAthena*, AFortPlayerController*, APlayerState*, AFortPawn*, UFortWeaponItemDefinition*, EDeathCause, char) = decltype(RemoveFromAlivePlayers)(InSDKUtils::GetImageBase() + 0x84929AC);
void (*ClientIsReadyToRespawnOG)(AFortPlayerControllerAthena* PC);
void ClientIsReadyToRespawn(AFortPlayerControllerAthena* Controller) {
	ClientIsReadyToRespawnOG(Controller);
	AFortPlayerStateAthena* PlayerStateAthena = Cast<AFortPlayerStateAthena>(Controller->PlayerState);
	Controller->WorldInventory->Inventory.ReplicatedEntries.Free();
	Controller->WorldInventory->Inventory.ItemInstances.Free();
	Controller->WorldInventory->Inventory.MarkArrayDirty();
	//((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->IsRespawningAllowed((AFortPlayerStateAthena*)Controller->PlayerState) && (!GetGamePhaseLogic()->SafeZoneIndicator || GetGamePhaseLogic()->SafeZoneIndicator->CurrentPhase <= 11)q
	if (true)
	{
		FFortRespawnData RespawnData = PlayerStateAthena->RespawnData;

		FTransform SpawnTransform{};
		SpawnTransform.Translation = RespawnData.RespawnLocation;
		SpawnTransform.Rotation = FRotToQuat(RespawnData.RespawnRotation);
		SpawnTransform.Scale3D = FVector(1, 1, 1);

		auto Pawn = (AFortPlayerPawnAthena*)GetGameMode()->SpawnDefaultPawnAtTransform(Controller, SpawnTransform);
		Controller->Possess(Pawn);
		Controller->RespawnPlayerAfterDeath(true);

		UpdateStormEffect(Controller);
		Pawn->bIsInAnyStorm = false;
		Pawn->bIsInsideSafeZone = true;
		Pawn->OnRep_IsInAnyStorm();
		Pawn->OnRep_IsInsideSafeZone();

		Pawn->SetHealth(100);
		if (bLateGame)
			Pawn->SetShield(100);
		else
			Pawn->SetShield(0);


		GiveStartingItems(Controller);
		if (bLateGame) {
			std::thread([PC = Controller]() {
				SetupLateGameItems(PC);
				}).detach();
		}
		PlayerStateAthena->RespawnData.bClientIsReady = true;

		auto PlayerState = Cast<AFortPlayerStateAthena>(Controller->PlayerState);
		if (PlayerState)
		{
			std::cout << "Respawn - Kills: " << PlayerState->KillScore << "\n";
		}

	}
}

inline void (*GetPlayerViewPoint)(const AFortPlayerControllerAthena*, FVector&, FRotator&);
void GetPlayerViewPointHook(const AFortPlayerControllerAthena* PlayerController, FVector& Location, FRotator& Rotation)
{
	if (!PlayerController)
		return;
	static auto SFName = Conv_StringToName(L"Spectating");

	if (PlayerController->StateName == SFName)
	{
		Location = PlayerController->LastSpectatorSyncLocation;
		Rotation = PlayerController->LastSpectatorSyncRotation;
	}
	else if (PlayerController->GetViewTarget())
	{
		Location = PlayerController->GetViewTarget()->K2_GetActorLocation();
		Rotation = PlayerController->GetControlRotation();
	}
	else
		return GetPlayerViewPoint(PlayerController, Location, Rotation);
}

void ServerPlayEmoteItemHook(AFortPlayerController* PC, UFortItemDefinition* EmoteAsset, float RandomEmoteNumber)
{

	if (!PC || !EmoteAsset || !PC->MyFortPawn)
		return;

	static UClass* DanceAbility = StaticLoadObject<UClass>("/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C");
	static UClass* SprayAbility = StaticLoadObject<UClass>("/Game/Abilities/Sprays/GAB_Spray_Generic.GAB_Spray_Generic_C");

	if (!DanceAbility || !SprayAbility)
		return;

	auto EmoteDef = (UAthenaDanceItemDefinition*)EmoteAsset;
	if (!EmoteDef)
		return;

	PC->MyFortPawn->bMovingEmote = EmoteDef->bMovingEmote;
	PC->MyFortPawn->EmoteWalkSpeed = EmoteDef->WalkForwardSpeed;
	PC->MyFortPawn->bMovingEmoteForwardOnly = EmoteDef->bMoveForwardOnly;
	PC->MyFortPawn->EmoteWalkSpeed = EmoteDef->WalkForwardSpeed;

	FGameplayAbilitySpec Spec{};
	UGameplayAbility* Ability = nullptr;
	if (EmoteAsset->IsA(UAthenaSprayItemDefinition::StaticClass()))
	{
		Ability = (UGameplayAbility*)SprayAbility->DefaultObject;
	}

	if (Ability == nullptr) {
		Ability = (UGameplayAbility*)DanceAbility->DefaultObject;
	}
	if (Ability) {
		FGameplayAbilitySpec Spec{};
		SpecConstructor(&Spec, Ability, 1, -1, EmoteAsset);
		GiveAbilityAndActivateOnce(((AFortPlayerStateAthena*)PC->PlayerState)->AbilitySystemComponent, &Spec.Handle, Spec, 0);
	}

}

void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int32 Count, bool bTrash)
{
	if (Count < 1)
		return;
	if (!PC->WorldInventory)
		return;
	FFortItemEntry* Entry = FindEntry(PC, ItemGuid);
	if (!Entry)
		return;
	if (Entry->Count < Count)
		return;
	auto pick = SpawnPickup(PC->Pawn->K2_GetActorLocation(), (UFortItemDefinition*)Entry->ItemDefinition, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Count, Entry->LoadedAmmo);
	pick->PawnWhoDroppedPickup = PC->MyFortPawn;
	RemoveItem(PC, ItemGuid, Count);
}

void ServerAcknowledgePossessionHook(AFortPlayerControllerAthena* PlayerController, APawn* P)
{
	PlayerController->AcknowledgedPawn = P;
}



void ServerAttemptAircraftJumpHook(const UFortControllerComponent_Aircraft* ControllerComponent, const FRotator& ClientRotation)
{
	if (!ControllerComponent)
		return;
	if (!AllowAircraftJumping)
		return;

	AFortPlayerControllerAthena* PlayerController = Cast<AFortPlayerControllerAthena>(ControllerComponent->GetOwner());
	if (!PlayerController || !PlayerController->IsInAircraft()) return;
	if (PlayerController->WorldInventory) {
		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Free();
		PlayerController->WorldInventory->Inventory.ItemInstances.Free();
		PlayerController->WorldInventory->Inventory.MarkArrayDirty();
		GiveStartingItems(PlayerController);
	}

	AFortGameModeAthena* GameMode = Cast<AFortGameModeAthena>(UWorld::GetWorld()->AuthorityGameMode);
	if (!GameMode) return;
	GameMode->RestartPlayer(PlayerController);
	if (PlayerController->MyFortPawn) {
		PlayerController->ClientSetRotation(ClientRotation, true);
		UpdateStormEffect(PlayerController);
		PlayerController->MyFortPawn->bIsInAnyStorm = false;
		PlayerController->MyFortPawn->bIsInsideSafeZone = true;
		PlayerController->MyFortPawn->OnRep_IsInAnyStorm();
		PlayerController->MyFortPawn->OnRep_IsInsideSafeZone();
		SetScalableFloatVal(PlayerController->MyFortPawn->ParachuteDeployTraceForGroundDistance, 10000.0f);
		if (bLateGame)
			SetScalableFloatVal(PlayerController->MyFortPawn->ParachuteDeployTraceForGroundDistance, 100.0f);
		if (bLateGame) {
			std::thread([PC = PlayerController]() {
				SetupLateGameItems(PC);
				}).detach();
		}

		PlayerController->MyFortPawn->BeginSkydiving(true);
		PlayerController->MyFortPawn->SetHealth(100);
		if (bLateGame)
			PlayerController->MyFortPawn->SetShield(100);
		else
			PlayerController->MyFortPawn->SetShield(0);
	}
}

void ServerSendZiplineState(AFortPlayerPawn* Pawn, FZiplinePawnState State)
{
	std::cout << "ServerSendZiplineState\n";
}

inline void (*OnDamageServer)(ABuildingSMActor*, float, FGameplayTagContainer&, FVector&, FHitResult&, AController*, AActor*, FGameplayEffectContextHandle&);
void OnDamageServerHook(ABuildingSMActor* BuildingActor, float Damage, FGameplayTagContainer& DamageTags, FVector& Momentum, FHitResult& HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle& EffectContext)
{
	if (!BuildingActor || !InstigatedBy || !BuildingActor->IsA(ABuildingSMActor::StaticClass()) || BuildingActor->bPlayerPlaced || !DamageCauser || !DamageCauser->IsA(AFortWeapon::StaticClass()) || !((AFortWeapon*)DamageCauser)->WeaponData)
		return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	auto ClassData = GetSparseData<FBuildingSMActorClassData>(BuildingActor->Class);

	if (!ClassData)
		return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	if (ClassData->BuildingResourceAmountOverride.RowName.ToString() == "NAME_None")
		return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	if (((AFortWeapon*)DamageCauser)->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
	{
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)InstigatedBy;
		if (!PlayerController)
			return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

		static UCurveTable* CurveTable = nullptr;
		CurveTable = StaticFindObject<UCurveTable>("/Game/Balance/DataTables/ResourceRates.ResourceRates");

		if (!CurveTable)
			return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

		float Average = 1.f;
		EEvaluateCurveTableResult Result = {};

		UDataTableFunctionLibrary::EvaluateCurveTableRow(CurveTable, ClassData->BuildingResourceAmountOverride.RowName, 0.f, &Result, &Average, FString());
		float FinalResourceCount = round(Average / (BuildingActor->GetMaxHealth() / Damage));

		if (FinalResourceCount > 0)
		{
			PlayerController->ClientReportDamagedResourceBuilding(BuildingActor, BuildingActor->ResourceType, FinalResourceCount, false, Damage == 100.f);
			auto Resource = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->ResourceType);
			auto MaxStack = 500;
			auto Item = FindEntry(InstigatedBy, Resource);

			if (Item)
			{
				Item->Count += FinalResourceCount;
				if (Item->Count > MaxStack)
				{
					SpawnPickup(InstigatedBy->Pawn->K2_GetActorLocation(), (UFortItemDefinition*)Item->ItemDefinition, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Item->Count - MaxStack, 0, InstigatedBy->MyFortPawn);
					Item->Count = MaxStack;
				}

				ModifyEntry(InstigatedBy, *Item);
			}
			else
			{
				if (FinalResourceCount > MaxStack)
				{
					SpawnPickup(InstigatedBy->Pawn->K2_GetActorLocation(), (UFortItemDefinition*)Item->ItemDefinition, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Item->Count - MaxStack, 0, InstigatedBy->MyFortPawn);
					FinalResourceCount = MaxStack;
				}

				GiveItem(InstigatedBy, Resource, FinalResourceCount, 0, 0);
			}

		}

		if (Damage == 100)
		{
			PlayerController->BroadcastRemoteClientInfo->RemoteWeakspotData.HitCount++;
		}
		else
			PlayerController->BroadcastRemoteClientInfo->RemoteWeakspotData.HitCount = 0;
	}

	return OnDamageServer(BuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
}

void ServerReturnToMainMenu(AFortPlayerControllerAthena* PlayerController) {
	PlayerController->ClientReturnToMainMenuWithTextReason(UKismetTextLibrary::Conv_StringToText(L""));
}

void(__fastcall* ClientOnPawnDiedOG)(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport);
void ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport& DeathReport) {
	auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	auto GameState = (AFortGameStateAthena*)GameMode->GameState;
	auto GamePhaseLogic = GetGamePhaseLogic();
	if (!PlayerController || GamePhaseLogic->GamePhase == EAthenaGamePhase::Warmup)
		return ClientOnPawnDiedOG(PlayerController, DeathReport);
	auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;

	if (PlayerController->WorldInventory) {
		for (auto& entry : PlayerController->WorldInventory->Inventory.ReplicatedEntries)
		{
			if (!entry.ItemDefinition)
				continue;
			if (!entry.ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) && (entry.ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) || entry.ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass()) || entry.ItemDefinition->IsA(UFortConsumableItemDefinition::StaticClass()) || entry.ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass())))
			{
				SpawnPickup(PlayerController->MyFortPawn->K2_GetActorLocation(), (UFortWorldItemDefinition*)entry.ItemDefinition, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, entry.Count, entry.LoadedAmmo, PlayerController->MyFortPawn);
			}
		}
	}

	auto KillerPlayerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	auto KillerPawn = (AFortPlayerPawnAthena*)DeathReport.KillerPawn.Get();

	PlayerState->PawnDeathLocation = PlayerController->MyFortPawn ? PlayerController->MyFortPawn->K2_GetActorLocation() : FVector();
	PlayerState->DeathInfo.bDBNO = PlayerController->MyFortPawn ? PlayerController->MyFortPawn->IsDBNO() : false;
	PlayerState->DeathInfo.DeathLocation = PlayerState->PawnDeathLocation;
	PlayerState->DeathInfo.DeathTags = PlayerController->MyFortPawn ? *(FGameplayTagContainer*)(__int64(PlayerController->MyFortPawn) + 0x2778) : DeathReport.Tags;
	PlayerState->DeathInfo.DeathCause = AFortPlayerStateAthena::ToDeathCause(PlayerState->DeathInfo.DeathTags, PlayerState->DeathInfo.bDBNO);
	if (PlayerState->DeathInfo.bDBNO)
		PlayerState->DeathInfo.Downer = KillerPlayerState;
	PlayerState->DeathInfo.FinisherOrDowner = KillerPlayerState;


	PlayerState->DeathInfo.bInitialized = true;
	PlayerState->OnRep_DeathInfo();

	if (KillerPlayerState && KillerPawn && KillerPawn->Controller && KillerPawn->Controller->IsA(AFortPlayerControllerAthena::StaticClass()) && KillerPawn->Controller != PlayerController)
	{
		KillerPlayerState->KillScore++;
		KillerPlayerState->OnRep_Kills();
		KillerPlayerState->TeamKillScore++;
		KillerPlayerState->OnRep_TeamKillScore();

		KillerPlayerState->ClientReportKill(PlayerState);
		KillerPlayerState->ClientReportTeamKill(KillerPlayerState->TeamKillScore);

		KillerPlayerState->ClientReportKill(PlayerState);
		KillerPlayerState->ClientReportTeamKill(KillerPlayerState->TeamKillScore);

		auto KillerPC = (AFortPlayerControllerAthena*)KillerPlayerState->Owner;

		if (PlayerController->MyFortPawn && KillerPlayerState && KillerPawn && KillerPawn->Controller != PlayerController && bLateGame)
		{
			// thx ploosh
			auto Handle = KillerPlayerState->AbilitySystemComponent->MakeEffectContext();
			FGameplayTag Tag;
			static auto Cue = Conv_StringToName(L"GameplayCue.Shield.PotionConsumed");
			Tag.TagName = Cue;
			KillerPlayerState->AbilitySystemComponent->NetMulticast_InvokeGameplayCueAdded(Tag, FPredictionKey(), Handle);
			KillerPlayerState->AbilitySystemComponent->NetMulticast_InvokeGameplayCueExecuted(Tag, FPredictionKey(), Handle);

			auto Health = KillerPawn->GetHealth();
			auto Shield = KillerPawn->GetShield();

			if (Health == 100)
			{
				Shield += Shield + 50;
			}
			else if (Health + 50 > 100)
			{
				Health = 100;
				Shield += (Health + 50) - 100;
			}
			else if (Health + 50 <= 100)
			{
				Health += 50;
			}

			KillerPawn->SetHealth(Health);
			KillerPawn->SetShield(Shield);

		}
	}
	if (GameState->IsRespawningAllowed(PlayerState))
	{
		auto GamePhaseLogic = GetGamePhaseLogic();
		auto SafeZoneIndicator = GamePhaseLogic ? GamePhaseLogic->SafeZoneIndicator : nullptr;
		int CurrentPhase = SafeZoneIndicator ? SafeZoneIndicator->CurrentPhase : 0;

		int AliveCount = GameMode->AlivePlayers.Num();

		bool bCanRespawn = false;

		if (reloadMode)
		{
			bCanRespawn = (CurrentPhase <= 7);
		}
		else
		{
			bCanRespawn = (CurrentPhase <= 7) && (AliveCount > 1);
		}

		std::cout << "bCanRespawn: " << bCanRespawn
			<< " | Phase: " << CurrentPhase
			<< " | AlivePlayers: " << AliveCount
			<< " | reloadMode: " << reloadMode << "\n";

		if (bCanRespawn)
		{
			std::cout << "Respawning allowed\n";
			return ClientOnPawnDiedOG(PlayerController, DeathReport);
		}
		else
		{
			std::cout << "Respawning blocked: last player or past phase 7\n";

			if (PlayerController->Pawn)
			{
				GameMode->AlivePlayers.Remove(PlayerState->PlayerId);

				PlayerController->Pawn->K2_DestroyActor();
				PlayerController->Pawn = nullptr;

				int AliveCount = GameMode->AlivePlayers.Num();
				std::cout << "Alive Players: " << AliveCount << "\n";
			}

			if (AliveCount == 1 && !bRespawning)
			{
				std::cout << "[DEBUG] AliveCount == 1 -> Checking for winner...\n";

				AFortPlayerControllerAthena* WinnerPC = nullptr;

				for (auto Player : GameMode->AlivePlayers)
				{
					if (Player)
					{
						std::cout << "[DEBUG] Found PlayerController: " << Player
							<< (Player == PlayerController ? " (Dead Player)" : " (Potential Winner)")
							<< "\n";

						if (Player != PlayerController)
						{
							WinnerPC = Cast<AFortPlayerControllerAthena>(Player);
						}
					}
					else
					{
						std::cout << "[DEBUG] Null player found in AlivePlayers array!\n";
					}
				}

				if (WinnerPC)
				{
					std::cout << "[DEBUG] WinnerPC found -> " << WinnerPC << "\n";

					auto WinnerPawn = WinnerPC->MyFortPawn;
					if (WinnerPawn)
					{
						std::cout << "[DEBUG] WinnerPawn found -> " << WinnerPawn << "\n";

						auto DeathCause = PlayerState ? PlayerState->DeathInfo.DeathCause : EDeathCause::Unspecified;

						std::cout << "[DEBUG] Triggering Win Effects...\n";
						std::cout << "---------------------------------------------\n";
						std::cout << " WINNER: " << WinnerPC->GetName() << "\n";
						std::cout << " PAWN: " << WinnerPawn->GetName() << "\n";
						std::cout << " DeathCause: " << static_cast<int>(DeathCause) << "\n";
						std::cout << "---------------------------------------------\n";

						WinnerPC->PlayWinEffects(WinnerPawn, nullptr, DeathCause, false, ERoundVictoryAnimation::ERoundVictoryAnimation_MAX);
						WinnerPC->ClientNotifyWon(WinnerPawn, nullptr, DeathCause);
						WinnerPC->ClientNotifyTeamWon(WinnerPawn, nullptr, DeathCause);
					}
					else
					{
						std::cout << "[DEBUG] WinnerPC->MyFortPawn is NULL! Cannot play win effects.\n";
					}
				}
				else
				{
					std::cout << "[DEBUG] No WinnerPC found! AlivePlayers may be empty or invalid.\n";
				}
			}
			else
			{
				std::cout << "[DEBUG] AliveCount != 1 -> Skipping winner check. Current: " << AliveCount << "\n";
			}

		}
	}

	if ((PlayerController->MyFortPawn ? !PlayerController->MyFortPawn->IsDBNO() : true))
	{
		PlayerState->Place = GameState->PlayersLeft;
		PlayerState->OnRep_Place();

		AFortWeapon* DamageCauser = nullptr;
		if (auto Projectile = DeathReport.DamageCauser.Get() ? Cast<AFortProjectileBase>(DeathReport.DamageCauser.Get()) : nullptr)
			DamageCauser = Projectile->GetOwnerWeapon();
		else if (auto Weapon = DeathReport.DamageCauser.Get() ? Cast<AFortWeapon>(DeathReport.DamageCauser.Get()) : nullptr)
			DamageCauser = Weapon;

		RemoveFromAlivePlayers(GameMode, PlayerController, KillerPlayerState == PlayerState ? nullptr : KillerPlayerState, KillerPawn, DamageCauser ? DamageCauser->WeaponData : nullptr, PlayerState->DeathInfo.DeathCause, 0);

		if (PlayerController->MyFortPawn && ((KillerPlayerState && KillerPlayerState->Place == 1) || PlayerState->Place == 1))
		{
			if (PlayerState->Place == 1)
			{
				KillerPlayerState = PlayerState;
				KillerPawn = (AFortPlayerPawnAthena*)PlayerController->MyFortPawn;
			}
			auto KillerPlayerController = (AFortPlayerControllerAthena*)KillerPlayerState->Owner;
			auto KillerWeapon = DamageCauser ? DamageCauser->WeaponData : nullptr;

			KillerPlayerController->PlayWinEffects(KillerPawn, KillerWeapon, PlayerState->DeathInfo.DeathCause, false, ERoundVictoryAnimation::ERoundVictoryAnimation_MAX);
			KillerPlayerController->ClientNotifyWon(KillerPawn, KillerWeapon, PlayerState->DeathInfo.DeathCause);
			KillerPlayerController->ClientNotifyTeamWon(KillerPawn, KillerWeapon, PlayerState->DeathInfo.DeathCause);

			GameState->WinningTeam = KillerPlayerState->TeamIndex;
			GameState->OnRep_WinningTeam();
			GameState->WinningPlayerState = KillerPlayerState;
			GameState->OnRep_WinningPlayerState();
		}
	}

	PlayerController->StateName = UKismetStringLibrary::Conv_StringToName(L"Spectating");

	return ClientOnPawnDiedOG(PlayerController, DeathReport);
}
bool(__fastcall* IsValid)(int* a1);
bool __fastcall sub_506990C(int* a1) {

	auto offset = __int64(_ReturnAddress()) - InSDKUtils::GetImageBase();

	if (offset > 0x838C4B4 && offset <= 0x838C795)
		return true;
	return IsValid(a1);
}
void (*SetupInventoryOG)(UFortControllerComponent_InventoryService* comp);
void SetupInventory(UFortControllerComponent_InventoryService* comp) {
	SetupInventoryOG(comp);

	if (!((AFortPlayerController*)comp->GetOwner())->WorldInventory) {
		((AFortPlayerController*)comp->GetOwner())->WorldInventory = SpawnActor<AFortInventory>({}, {}, comp->GetOwner());
		//((AFortPlayerControllerAthena*)comp->GetOwner())->OnMinigamePlayModeChanged();
	}
}

void (*ServerLoadingScreenDroppedOG)(AFortPlayerControllerAthena* PC);
void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC) {
	ServerLoadingScreenDroppedOG(PC);

}
__int64 (*OnTacticalSprintStartedOG)(AFortPlayerPawn* Pawn);
__int64 OnTacticalSprintStartedHK(AFortPlayerPawn* Pawn) {
	auto res = OnTacticalSprintStartedOG(Pawn);
	std::cout << "Started Tactical Sprint\n";
	return res;
}
void MovingEmoteStopped(UObject* Context, FFrame& Stack)
{
	Stack.IncrementCode();

	AFortPawn* Pawn = (AFortPawn*)Context;
	Pawn->bMovingEmote = false;
	Pawn->bMovingEmoteForwardOnly = false;
	Pawn->bMovingEmoteFollowingOnly = false;
}


void (*ServerReadyToStartMatchOG)(AFortPlayerControllerAthena* PlayerController);
void ServerReadyToStartMatch(AFortPlayerControllerAthena* PlayerController) {
	ServerReadyToStartMatchOG(PlayerController);
	if (PlayerController) {
		auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;

		if (PlayerState) {
			if (isRunning) {
				KickPlayer(PlayerController);
			}
		}
	}
	if (GetGamePhaseLogic()->GamePhase >= EAthenaGamePhase::Aircraft) {
		auto CenterPos = GetGameState()->MapInfo->GetMapCenter();
		if (GetGamePhaseLogic()->SafeZoneIndicator)
			CenterPos = GetGamePhaseLogic()->SafeZoneIndicator->GetSafeZoneCenter();
		FVector Pos = FVector(CenterPos.X, CenterPos.Y, 15000);
		UpdateStormEffect(PlayerController);
		PlayerController->MyFortPawn->K2_TeleportTo(Pos, FRotator());

		PlayerController->MyFortPawn->bIsInAnyStorm = false;
		PlayerController->MyFortPawn->bIsInsideSafeZone = true;
		PlayerController->MyFortPawn->OnRep_IsInAnyStorm();
		PlayerController->MyFortPawn->OnRep_IsInsideSafeZone();

		PlayerController->MyFortPawn->SetHealth(100);
		PlayerController->MyFortPawn->SetShield(0);
		PlayerController->MyFortPawn->BeginSkydiving(true);
	}
	if (!PlayerController->WorldInventory) {
		PlayerController->WorldInventory = SpawnActor<AFortInventory>({}, {}, PlayerController);
	}

	auto PlayerState = (AFortPlayerStateAthena*)(PlayerController->PlayerState);
	auto GameState = (AFortGameStateAthena*)(UWorld::GetWorld()->GameState);
	if (!PlayerState)
		return;

	GiveDefaultAbilitySet(PlayerController);
	//reinterpret_cast<void(*)(void*)>(InSDKUtils::GetImageBase() + 0x8DFE598)(GetInterfaceAddress(PlayerState, IAbilitySystemInterface::StaticClass())); // ability stuff
	GiveStartingItems((AFortPlayerControllerAthena*)PlayerController);
	if (PlayerController->CosmeticLoadoutPC.Character)
	{
		PlayerState->HeroType = PlayerController->CosmeticLoadoutPC.Character->HeroDefinition;
		UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
	}


	FGameMemberInfo Info{ -1,-1,-1 };
	Info.MemberUniqueId = PlayerState->UniqueID;
	Info.SquadId = PlayerState->SquadId;
	Info.TeamIndex = PlayerState->TeamIndex;
	GameState->GameMemberInfoArray.Members.Add(Info);
	GameState->GameMemberInfoArray.MarkItemDirty(Info);
}
void ServerCreateBuildingActorHook(AFortPlayerControllerAthena* PlayerController, const FCreateBuildingActorData& CreateBuildingData)
{

	if (!PlayerController || PlayerController->IsInAircraft())
		return;

	TArray<ABuildingActor*> ExistingBuildings;
	EFortBuildPreviewMarkerOptionalAdjustment a;

	if (!PlayerController->BroadcastRemoteClientInfo) return;
	auto RemoteBuildableClass = ((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->AllPlayerBuildableClassesIndexLookup.SearchForKey([&](UClass* Class, int32 Handle) {
		return Handle == CreateBuildingData.BuildingClassHandle;
		});

	if (!RemoteBuildableClass)
	{
		return;
	}
	auto res = GetGameState()->StructuralSupportSystem->CanAddBuildingActorClassToGrid(GetWorld(), RemoteBuildableClass->Get(), CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ExistingBuildings, &a, false);

	if (res != EFortStructuralGridQueryResults::CanAdd)
	{
		ExistingBuildings.Free();
		return;
	}

	ABuildingSMActor* NewBuildingActor = Cast<ABuildingSMActor>(SpawnActor(CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, RemoteBuildableClass->Get()));

	if (!NewBuildingActor)
		return;

	if (!PlayerController->PlayerState)
		return;

	NewBuildingActor->CurrentBuildingLevel = CreateBuildingData.BuildingClassData.UpgradeLevel;
	NewBuildingActor->OnRep_CurrentBuildingLevel();
	NewBuildingActor->SetMirrored(CreateBuildingData.bMirrored);
	NewBuildingActor->TeamIndex = Cast<AFortPlayerStateAthena>(PlayerController->PlayerState)->TeamIndex;
	NewBuildingActor->Team = EFortTeam((Cast<AFortPlayerStateAthena>(PlayerController->PlayerState)->TeamIndex));
	NewBuildingActor->OnRep_Team();
	NewBuildingActor->bPlayerPlaced = true;

	NewBuildingActor->InitializeKismetSpawnedBuildingActor(NewBuildingActor, PlayerController, NewBuildingActor->bPlayerPlaced, nullptr, true);
	if (NewBuildingActor->bPlayerPlaced)
		Builds.Add(NewBuildingActor);
	for (AActor* ExistingBuilding : ExistingBuildings)
	{
		ExistingBuilding->K2_DestroyActor();
	}

	UFortItemDefinition* ItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(NewBuildingActor->ResourceType);
	if (!ItemDefinition) return;

	FFortItemEntry* ItemEntry = FindEntry(PlayerController, ItemDefinition);
	if (!ItemEntry) return;

	if (!PlayerController->bBuildFree)
	{
		RemoveItem(PlayerController, ItemEntry->ItemGuid, 10);
	}

	ExistingBuildings.Free();
}

void ServerBeginEditingBuildingActorHook(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit)
{
	if (!PlayerController || !PlayerController->MyFortPawn || !BuildingActorToEdit)
		return;

	AFortPlayerStateAthena* PlayerState = Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);

	BuildingActorToEdit->EditingPlayer = PlayerState;
	BuildingActorToEdit->OnRep_EditingPlayer();

	static auto EditToolDefinition = StaticFindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
	if (!PlayerController->MyFortPawn->CurrentWeapon || PlayerController->MyFortPawn->CurrentWeapon->WeaponData != EditToolDefinition)
	{
		FFortItemEntry* ItemEntry = FindEntry(PlayerController, EditToolDefinition);

		if (!ItemEntry)
			return;

		PlayerController->ServerExecuteInventoryItem(ItemEntry->ItemGuid);
	}

	AFortWeap_EditingTool* EditTool = Cast<AFortWeap_EditingTool>(PlayerController->MyFortPawn->CurrentWeapon);

	if (EditTool)
	{
		EditTool->EditActor = BuildingActorToEdit;
		EditTool->OnRep_EditActor();
	}
}

void ServerEditBuildingActorHook(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<class ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored)
{
	if (!PlayerController || !BuildingActorToEdit || !NewBuildingClass || BuildingActorToEdit->bDestroyed || BuildingActorToEdit->EditingPlayer != (AFortPlayerStateAthena*)PlayerController->PlayerState)
		return;

	auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;

	if (PlayerState && PlayerState->TeamIndex != BuildingActorToEdit->TeamIndex)
		return;

	BuildingActorToEdit->EditingPlayer = nullptr;

	if (auto NewBuilding = ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass.Get(), BuildingActorToEdit->CurrentBuildingLevel, RotationIterations, bMirrored, PlayerController))
	{
		NewBuilding->bPlayerPlaced = true;
	}
}

void ServerEndEditingBuildingActorHook(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToStopEditing)
{
	if (!PlayerController || !BuildingActorToStopEditing || BuildingActorToStopEditing->EditingPlayer != (AFortPlayerStateAthena*)PlayerController->PlayerState || !PlayerController->MyFortPawn)
		return;

	BuildingActorToStopEditing->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	BuildingActorToStopEditing->EditingPlayer = nullptr;

	static auto EditToolDefinition = StaticFindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");

	if (!PlayerController->MyFortPawn->CurrentWeapon || PlayerController->MyFortPawn->CurrentWeapon->WeaponData != EditToolDefinition)
	{
		FFortItemEntry* ItemEntry = FindEntry(PlayerController, EditToolDefinition);

		if (!ItemEntry)
			return;

		PlayerController->ServerExecuteInventoryItem(ItemEntry->ItemGuid);
	}

	AFortWeap_EditingTool* EditTool = Cast<AFortWeap_EditingTool>(PlayerController->MyFortPawn->CurrentWeapon);

	if (EditTool)
	{
		EditTool->EditActor = nullptr;
		EditTool->OnRep_EditActor();
	}
}

void (*OnReloadOG)(AFortWeapon* Weapon, int RemoveCount);
void OnReload(AFortWeapon* Weapon, int RemoveCount)
{
	OnReloadOG(Weapon, RemoveCount);
	auto WeaponDef = Weapon->WeaponData;
	if (!WeaponDef)
		return;

	auto AmmoDef = WeaponDef->GetAmmoWorldItemDefinition_BP();
	if (!AmmoDef)
		return;
	AFortPlayerPawnAthena* Pawn = (AFortPlayerPawnAthena*)Weapon->GetOwner();
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
	if (!PC->IsA(AFortPlayerControllerAthena::StaticClass()))
		return;

	if (PC->bInfiniteAmmo)
		return;

	RemoveItem(PC, AmmoDef, RemoveCount);
	UpdateLoadedAmmo(PC, Weapon, RemoveCount);

	return;
}

void ServerExecuteInventoryItemHook(const AFortPlayerControllerAthena* PC, const FGuid& Guid)
{
	if (!PC || !PC->MyFortPawn || PC->IsInAircraft())
		return;
	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)
			continue;
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
		{
			UFortWeaponItemDefinition* WeaponDef = Cast<UFortWeaponItemDefinition>(PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition);
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()))
			{
				WeaponDef = StaticLoadObject<UFortWeaponItemDefinition>(Cast<UFortGadgetItemDefinition>(PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)->WeaponItemDefinition.ObjectID.GetAssetPathName());
			}
			if (!WeaponDef)
				return;
			PC->MyFortPawn->EquipWeaponDefinition(WeaponDef, Guid, FGuid(), false);
			break;
		}
	}
}
inline void SendMessageToConsole(AFortPlayerControllerAthena* PlayerController, const FString& Msg)
{
	PlayerController->ClientMessage(Msg, FName(), 1);
}
inline void SendMessageToConsole(AFortPlayerControllerAthena* PlayerController, std::string Msg)
{
	PlayerController->ClientMessage(std::wstring(Msg.begin(), Msg.end()).c_str(), FName(), 1);
}
bool IsAdmin(AFortPlayerStateAthena* PlayerState) {
	return true;
}
void ServerCheat(AFortPlayerControllerAthena* PC, FString Msg)
{
	static vector<string> Admins;
	auto PlayerState = Cast<AFortPlayerStateAthena>(PC->PlayerState);
	auto PlayerName = PlayerState->GetPlayerName().ToString();
	auto Pawn = (AFortPlayerPawnAthena*)PC->Pawn;
	if (!IsAdmin(PlayerState) && std::find(Admins.begin(), Admins.end(), PlayerState->GetPlayerName().ToString()) == Admins.end())
		return;
	bool isMsgEmpty = Msg.Num() <= 0;

	std::vector<std::string> Arguments;
	std::string OldMsg = "";
	if (!isMsgEmpty)
		OldMsg = Msg.ToString();

	if (!isMsgEmpty)
	{
		auto Message = Msg.ToString();
		while (Message.find(" ") != std::string::npos)
		{
			auto arg = Message.substr(0, Message.find(' '));
			Arguments.push_back(arg);
			Message.erase(0, Message.find(' ') + 1);
		}
		Arguments.push_back(Message);
	}
	auto NumArgs = Arguments.size() == 0 ? 0 : Arguments.size() - 1;

	if (Arguments.size() >= 1)
	{
		auto& Command = Arguments[0];
		std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);

		if (Command == "startaircraft")
		{
			StartAircraftPhase(GetGameMode());
		}


		if (Command == "randombot") {
			auto Bot = GetGameMode()->AliveBots[rand() % GetGameMode()->AliveBots.Num()];
			FRotator Rot = FRotator();
			PC->MyFortPawn->K2_SetActorLocation(Bot->PlayerBotPawn->K2_GetActorLocation(), false, nullptr, true);
			PC->MyFortPawn->K2_SetActorRotation(Rot, true);
			PC->MyFortPawn->K2_TeleportTo(Bot->PlayerBotPawn->K2_GetActorLocation(), Rot);
		}

		if (Command == "op") {
			if (NumArgs < 1) {
				return;
			}
			Admins.push_back(Arguments[1]);
		}
		if (Command == "test1") {
			std::ofstream ModStationFile("ModStations.txt");

			for (int i = 0; i < UObject::GObjects->Num(); i++)
			{
				auto Object = UObject::GObjects->GetByIndex(i);

				if (!Object)
					continue;

				if (Object->GetFullName().contains("ModStation"))
					ModStationFile << Object->GetFullName() << "\n";
			}

			ModStationFile << "\n--- Actors Found ---\n";
			TArray<AActor*> WeaponMods;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), &WeaponMods);
			for (auto Actor : WeaponMods) {
				if (Actor->GetFullName().contains("ModStation"))
					ModStationFile << Actor->GetFullName() << "\n";
			}

			SendMessageToConsole(PC, "Mod stations saved to ModStations.txt");
		}

		if (Command == "spawnmodstation")
		{
			UClass* ModStationClass = nullptr;

			for (int i = 0; i < UObject::GObjects->Num(); i++)
			{
				auto Object = UObject::GObjects->GetByIndex(i);
				if (!Object) continue;

				std::string FullName = Object->GetFullName();

				if (FullName.find("FortWeaponModStationBase") != std::string::npos &&
					Object->IsA(UClass::StaticClass()))
				{
					ModStationClass = (UClass*)Object;
					SendMessageToConsole(PC, "Trying to spawn: " + FullName);
					break;
				}
			}

			if (ModStationClass)
			{
				FVector SpawnLocation = PC->Pawn->K2_GetActorLocation() + FVector(300.0f, 0.0f, 0.0f);
				SpawnActor(SpawnLocation, FRotator(), ModStationClass);
				SendMessageToConsole(PC, "Spawn attempted!");
				UpdateSpeialActors();
			}
			else
			{
				SendMessageToConsole(PC, "FortWeaponModStationBase not found!");
			}
		}

		if (Command == "giveitem")
		{
			if (NumArgs < 1)
			{
				return;
			}

			auto& WeaponName = Arguments[1];
			int Count = 1;

			if (NumArgs >= 2)
				Count = std::stoi(Arguments[2]);

			auto WID = StaticFindObject<UFortWorldItemDefinition>(WeaponName, nullptr, (UObject*)-1);



			GiveItem(PC, WID, Count);

		}
		if (Command == "getlist")
		{
			std::ofstream WeaponsFile("Weapons.txt");

			auto SClass = UFortWorldItemDefinition::StaticClass();
			for (int i = 0; i < SClass->GObjects->Num(); i++) {
				UObject* obj = SClass->GObjects->GetByIndex(i);
				if (obj && obj->IsA(UFortWorldItemDefinition::StaticClass())) {
					std::string name = obj->GetFullName();
					WeaponsFile << name << "\n";

				}
			}
			//
			auto SClass2 = UFortWeaponModItemDefinition::StaticClass();
			for (int i = 0; i < SClass2->GObjects->Num(); i++) {
				UObject* obj = SClass2->GObjects->GetByIndex(i);
				if (obj && obj->IsA(UFortWeaponModItemDefinition::StaticClass())) {
					std::string name = obj->GetFullName();
					WeaponsFile << name << "\n";

				}
			}
		}
		if (Command == "playerlist")
		{
			for (auto TargetPlayer : GetGameMode()->AlivePlayers)
			{
				auto TargetPlayerState = Cast<AFortPlayerStateAthena>(TargetPlayer->PlayerState);
				auto NickName = TargetPlayerState->GetPlayerName().ToString();
				auto Ip = TargetPlayerState->SavedNetworkAddress.ToString();
				auto Res = NickName + " " + Ip;
				SendMessageToConsole(PC, Res);
			}

		}
		if (Command == "spawn") {
			if (NumArgs < 1)
			{
				return;
			}
			auto Path = Arguments[1];
			auto Class = StaticLoadObject<UClass>(Path);
			if (Class)
				SpawnActor(Pawn->K2_GetActorLocation(), FRotator(), Class);
			else
				SendMessageToConsole(PC, "Cant find actor to spawn!");
			UpdateSpeialActors();

		}
		if (Command == "spawntornado") {
			auto Class = StaticLoadObject<UClass>("/Superstorm/Tornado/BP_Tornado.BP_Tornado_C", UBlueprintGeneratedClass::StaticClass());
			SpawnActor(Pawn->K2_GetActorLocation(), FRotator(), Class);
			UpdateSpeialActors();
		}
		if (Command == "unban")
		{
			if (NumArgs < 1)
			{
				return;
			}

			SendMessageToConsole(PC, "Succesfully Un Banned! " + Arguments[1]);
		}
		if (Command == "crash") {
			throw std::out_of_range("Index was out of range!");
		}
		if (Command == "kicknick")
		{
			if (NumArgs < 1)
			{
				return;
			}
			for (auto TargetPlayer : GetGameMode()->AlivePlayers)
			{
				auto TargetPlayerState = Cast<AFortPlayerStateAthena>(TargetPlayer->PlayerState);
				auto NickName = TargetPlayerState->GetPlayerName().ToString();
				auto Ip = TargetPlayerState->SavedNetworkAddress.ToString();
				if (NickName == Arguments[1]) {
					KickPlayer(TargetPlayer);
					SendMessageToConsole(PC, "Succesfully Kicked! " + NickName + " " + Ip);
				}
			}
		}
		if (Command == "kickip")
		{
			if (NumArgs < 1)
			{
				return;
			}
			for (auto TargetPlayer : GetGameMode()->AlivePlayers)
			{
				auto TargetPlayerState = Cast<AFortPlayerStateAthena>(TargetPlayer->PlayerState);
				auto NickName = TargetPlayerState->GetPlayerName().ToString();
				auto Ip = TargetPlayerState->SavedNetworkAddress.ToString();
				if (Ip == Arguments[1]) {
					KickPlayer(TargetPlayer);
					SendMessageToConsole(PC, "Succesfully Kicked! " + NickName + " " + Ip);
				}
			}
		}


		if (Command == "freebuild")
		{
			PC->bBuildFree = !PC->bBuildFree;
		}
		if (Command == "spawnbot")
		{
			static auto SpawnerDataClass = StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_AISpawnerData_Phoebe.BP_AISpawnerData_Phoebe_C");
			auto DefaultObject = (UFortAthenaAIBotSpawnerData*)SpawnerDataClass->DefaultObject;
			auto ComponentList = DefaultObject->CreateComponentList(DefaultObject);
			auto Transform = FTransform();
			Transform.Translation = PC->Pawn->K2_GetActorLocation();
			Transform.Rotation = FQuat();
			Transform.Scale3D = FVector(1, 1, 1);

			((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
		}

		if (Command == "spawnlama")
		{
			static auto SpawnerDataClass = StaticLoadObject<UClass>("/Labrador/AISpawnerData/BP_AISpawnerData_Labrador.BP_AISpawnerData_Labrador_C");
			auto DefaultObject = (UFortAthenaAIBotSpawnerData*)SpawnerDataClass->DefaultObject;
			auto ComponentList = DefaultObject->CreateComponentList(DefaultObject);
			auto Transform = FTransform();
			Transform.Translation = PC->Pawn->K2_GetActorLocation();
			Transform.Rotation = FQuat();
			Transform.Scale3D = FVector(1, 1, 1);

			((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
		}

		if (Command == "spawnboss") {
			auto PaprikaSpawnerClass = StaticLoadObject<UClass>("/PaprikaBosses/SpawnerData/ZebraScramble/BP_AIBSD_PaprikaBoss_ZebraScramble.BP_AIBSD_PaprikaBoss_ZebraScramble_C", UBlueprintGeneratedClass::StaticClass());
			auto SpawnerData = (UFortAthenaAIBotSpawnerData*)PaprikaSpawnerClass->DefaultObject;

			auto ComponentList = SpawnerData->CreateComponentList(SpawnerData);
			auto Transform = FTransform();
			Transform.Translation = PC->Pawn->K2_GetActorLocation();
			Transform.Rotation = FQuat();
			Transform.Scale3D = FVector(1, 1, 1);

			((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
		}
		if (Command == "patrol") {
			auto ZebraScrambleSpawnerClass = StaticLoadObject<UClass>("/PaprikaBosses/SpawnerData/ZebraScramble/BP_AIBSD_PaprikaBoss_ZebraScramble.BP_AIBSD_PaprikaBoss_ZebraScramble_C", UBlueprintGeneratedClass::StaticClass());
			auto MadameMothSpawnerClass = StaticLoadObject<UClass>("/PaprikaBosses/SpawnerData/MadameMoth/BP_AIBSD_PaprikaBoss_MadameMoth.BP_AIBSD_PaprikaBoss_MadameMoth_C", UBlueprintGeneratedClass::StaticClass());
			auto DiamondHeartSpawnerClass = StaticLoadObject<UClass>("/PaprikaBosses/SpawnerData/DiamondHeart/BP_AIBSD_PaprikaBoss_DiamondHeart.BP_AIBSD_PaprikaBoss_DiamondHeart_C", UBlueprintGeneratedClass::StaticClass());
			auto BengalBasherSpawnerClass = StaticLoadObject<UClass>("/PaprikaBosses/SpawnerData/BengalBasher/BP_AIBSD_PaprikaBoss_BengalBasher.BP_AIBSD_PaprikaBoss_BengalBasher_C", UBlueprintGeneratedClass::StaticClass());

			TArray<AActor*> PatrolPoints;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortAthenaPatrolPathPointProvider::StaticClass(), &PatrolPoints);
			SendMessageToConsole(PC, std::to_string(PatrolPoints.Num()));
			for (auto Actor : PatrolPoints) {
				auto PatrolPath = (AFortAthenaPatrolPathPointProvider*)Actor;
				if (PatrolPath->FiltersTags.GameplayTags.Num() == 0)
					continue;
				auto PathName = PatrolPath->FiltersTags.GameplayTags[0].TagName.ToString();
				SendMessageToConsole(PC, PathName);
				if (PathName.contains("BengalBasher")) {
					auto SpawnerData = (UFortAthenaAIBotSpawnerData*)BengalBasherSpawnerClass->DefaultObject;
					auto ComponentList = SpawnerData->CreateComponentList(SpawnerData);
					auto Transform = PatrolPath->AssociatedPatrolPath->PatrolPoints[0]->GetTransform();
					((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
				}
				else if (PathName.contains("ZebraScramble")) {
					auto SpawnerData = (UFortAthenaAIBotSpawnerData*)ZebraScrambleSpawnerClass->DefaultObject;
					auto ComponentList = SpawnerData->CreateComponentList(SpawnerData);
					auto Transform = PatrolPath->AssociatedPatrolPath->PatrolPoints[0]->GetTransform();
					((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
				}
				else if (PathName.contains("MadameMoth")) {
					auto SpawnerData = (UFortAthenaAIBotSpawnerData*)MadameMothSpawnerClass->DefaultObject;
					auto ComponentList = SpawnerData->CreateComponentList(SpawnerData);
					auto Transform = PatrolPath->AssociatedPatrolPath->PatrolPoints[0]->GetTransform();
					((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
				}
				else if (PathName.contains("DiamondHeart")) {
					auto SpawnerData = (UFortAthenaAIBotSpawnerData*)DiamondHeartSpawnerClass->DefaultObject;
					auto ComponentList = SpawnerData->CreateComponentList(SpawnerData);
					auto Transform = PatrolPath->AssociatedPatrolPath->PatrolPoints[0]->GetTransform();
					((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->AISpawner->RequestSpawn(ComponentList, Transform, false);
				}
			}
			UpdateSpeialActors();
		}
		if (Command == "spec") {
			UpdateSpeialActors();
		}

	}
}
#endif // !PLAYERCONTROLLER
