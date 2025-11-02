#include <unordered_map>
#ifndef AI
#define AI
#pragma once
#include "Utils.h"
#include "Abilities.h"
#include "Looting.h"
#include "MedallionRegen.h"

static std::unordered_map<std::string, std::string> BossWeaponMap = {
	// Zebra Scramble Boss
	{"BP_PaprikaBoss_ZebraScramble_C", "WID_Assault_Paprika_DPS_Athena_UR_Boss.WID_Assault_Paprika_DPS_Athena_UR_Boss"},
	// Madame Moth Boss
	{"BP_PaprikaBoss_MadameMoth_C", "WID_Sniper_Paprika_Athena_SR.WID_Sniper_Paprika_Athena_SR"},
	// Diamond Heart Boss
	{"BP_PaprikaBoss_DiamondHeart_C", "WID_Shotgun_Pump_Paprika_Athena_R.WID_Shotgun_Pump_Paprika_Athena_R"},
	// Bengal Basher Boss
	{"BP_PaprikaBoss_BengalBasher_C", "WID_Assault_Paprika_DPS_Athena_UR_Boss.WID_Assault_Paprika_DPS_Athena_UR_Boss"}
	// Add more bosses here as needed:
	// {"BossClassName", "WeaponID"}
};

__m128 (__fastcall* GetTrackingModifierInternalOG)(UFortAthenaAIBotAimingDigestedSkillSet* a1, FDigestedTrackingOffsetModifierCurves* a2, __int64 a3, __int64 a4);
__m128 __fastcall GetTrackingModifierInternalHK(UFortAthenaAIBotAimingDigestedSkillSet* a1, FDigestedTrackingOffsetModifierCurves* a2, __int64 a3, __int64 a4) {
	auto res = GetTrackingModifierInternalOG(a1, a2, a3, a4);
	return res;
}
void* (*ApplyCharacterCustomizationOriginal)(AFortPlayerState* a1, AFortPawn* a3) = decltype(ApplyCharacterCustomizationOriginal)(__int64(GetModuleHandle(0)) + 0x96EF4C4);
void (*AIBotBehaviorOnSpawnedOG)(UFortAthenaAISpawnerDataComponent_AIBotBehavior* AIBotBehavior, AFortPlayerPawn* Pawn);

void AIBotBehaviorOnSpawned(UFortAthenaAISpawnerDataComponent_AIBotBehavior* AIBotBehavior, AFortPlayerPawn* Pawn)
{
	auto Controller = (AFortAthenaAIBotController*)Pawn->GetController();
	//*(bool*)(__int64(Controller->CachedAIServicePlayerBots) + 0x930) = true;

	if (!Controller->BrainComponent || !Controller->BrainComponent->IsA(UBehaviorTreeComponent::StaticClass()))
	{
		Controller->BrainComponent = (UBrainComponent*)Controller->AddComponentByClass(UAthenaBehaviorTreeComponent::StaticClass(), true, FTransform(), false);
	}
	return AIBotBehaviorOnSpawnedOG(AIBotBehavior, Pawn);
}

void (*AIBotCosmeticOnSpawnedOG)(UFortAthenaAISpawnerDataComponent_CosmeticLoadout* AIBotBehavior, AFortPlayerPawn* Pawn);
void AIBotCosmeticOnSpawned(UFortAthenaAISpawnerDataComponent_CosmeticLoadout* AIBotCosmetic, AFortPlayerPawn* Pawn)
{
	AIBotCosmeticOnSpawnedOG(AIBotCosmetic, Pawn);
	FFortAthenaLoadout Loadout;
	AIBotCosmetic->GetLoadout(&Loadout);
	if (Loadout.Character)
	{
		Pawn->CosmeticLoadout = Loadout;
		ApplyCharacterCustomizationOriginal((AFortPlayerState*)Pawn->PlayerState, Pawn);
		Pawn->ForceNetUpdate();
		Pawn->GetController()->ForceNetUpdate();
	}
}
void (*OnPossesedPawnDiedOG)(AFortAthenaAIBotController* PC, AActor* DamagedActor, float Damage, AController* InstigatedBy, AActor* DamageCauser, FVector HitLocation, UPrimitiveComponent* HitComp, FName BoneName, FVector Momentum);
void OnPossesedPawnDiedHK(AFortAthenaAIBotController* PC, AActor* DamagedActor, float Damage, AController* InstigatedBy, AActor* DamageCauser, FVector HitLocation, UPrimitiveComponent* HitComp, FName BoneName, FVector Momentum)
{
	if (PC && PC->Pawn && PC->LootItemsToDropOnDeath.IsValid()) {
		for (auto Entry : PC->LootItemsToDropOnDeath)
		{
			SpawnPickup(PC->Pawn->K2_GetActorLocation(), (UFortWorldItemDefinition*)Entry.ItemDefinition, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, Entry.Count, Entry.LoadedAmmo, PC->PlayerBotPawn);
		}
	}

	return OnPossesedPawnDiedOG(PC, DamagedActor, Damage, InstigatedBy, DamageCauser, HitLocation, HitComp, BoneName, Momentum);
}
void (*AIBotInventoryOnSpawnedOG)(UFortAthenaAISpawnerDataComponent_InventoryBase* AIBotInventory, AFortPlayerPawn* Pawn);
void AIBotInventoryOnSpawned(UFortAthenaAISpawnerDataComponent_InventoryBase* InventoryBase, AFortPlayerPawn* Pawn)
{
	printf(__FUNCTION__);
	auto Controller = (AFortAthenaAIBotController*)Pawn->Controller;
	if (!Controller)
		return;
	if (!Controller->Inventory)
		Controller->Inventory = SpawnActor<AFortInventory>(AFortInventory::StaticClass(), {}, {}, Controller);
	if (Controller->IsA(ABP_PhoebePlayerController_C::StaticClass())) {
		for (auto& Item : ((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode)->StartingItems)
		{
			UFortWorldItem* BP = Cast<UFortWorldItem>(((UFortItemDefinition*)Item.Item)->CreateTemporaryItemInstanceBP(Item.Count, 0));
			FFortItemEntry& Entry = BP->ItemEntry;
			Controller->Inventory->Inventory.ReplicatedEntries.Add(Entry);
			Controller->Inventory->Inventory.ItemInstances.Add(BP);
			Controller->Inventory->Inventory.MarkItemDirty(Entry);
			Controller->Inventory->HandleInventoryLocalUpdate();
		}
	}
	for (auto& Item : InventoryBase->Items)
	{
		if (!Item.Item) continue;
		auto ItemDef = (UFortItemDefinition*)Item.Item;
		UFortWorldItem* BP = Cast<UFortWorldItem>((ItemDef)->CreateTemporaryItemInstanceBP(Item.Count, 0));
		FFortItemEntry& Entry = BP->ItemEntry;
		Controller->Inventory->Inventory.ReplicatedEntries.Add(Entry);
		Controller->Inventory->Inventory.ItemInstances.Add(BP);
		Controller->Inventory->Inventory.MarkItemDirty(Entry);
		Controller->Inventory->HandleInventoryLocalUpdate();
	}
	if (InventoryBase->LootInfo.DataTable && InventoryBase->LootInfo.RowName.ComparisonIndex > 0) {
		auto Value = InventoryBase->LootInfo.DataTable->RowMap.Search([InventoryBase](FName& Key, uint8_t* Value) {return InventoryBase->LootInfo.RowName == Key && Value; });
		FFortAthenaAILootInfoDataTableRow* LootInfo = Value ? *(FFortAthenaAILootInfoDataTableRow**)Value : nullptr;
		if (LootInfo) {
			for (auto LootDroppingBehaviour : LootInfo->LootDroppingBehaviors) {
				for (auto LootTier : LootDroppingBehaviour.LootTiers) {
					auto LootDrops = PickLootDrops(LootTier, 0);
					for (auto Entry : LootDrops) {
						Controller->LootItemsToDropOnDeath.Add(Entry);
					}
				}
			}
		}
	}

	Controller->Inventory->ForceNetUpdate();
	Controller->EquipMeleeWeapon();

	// --- BOSS LOGIC START ---
	// Detect if this pawn is a boss (by class name)
	std::string BossClassName = Pawn->GetFullName();
	bool isBoss = BossClassName.find("PaprikaBoss") != std::string::npos;
	if (isBoss) {
		// Try to find a weapon for this boss
		std::string WeaponName;
		for (const auto& pair : BossWeaponMap) {
			if (BossClassName.find(pair.first) != std::string::npos) {
				WeaponName = pair.second;
				break;
			}
		}
		// Give boss a strong ranged weapon if it doesn't have one
		bool hasRanged = false;
		FFortItemEntry* EquippedEntry = nullptr;
		UFortWeaponRangedItemDefinition* EquippedWeaponDef = nullptr;
		for (auto& Entry : Controller->Inventory->Inventory.ReplicatedEntries) {
			if (Entry.ItemDefinition && Entry.ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass())) {
				hasRanged = true;
				EquippedEntry = &Entry;
				EquippedWeaponDef = (UFortWeaponRangedItemDefinition*)Entry.ItemDefinition;
				break;
			}
		}
		if (!hasRanged) {
			if (!WeaponName.empty()) {
				auto WeaponDef = UObject::FindObject<UFortWeaponRangedItemDefinition>(WeaponName);
				if (WeaponDef) {
					UFortWorldItem* BP = Cast<UFortWorldItem>(WeaponDef->CreateTemporaryItemInstanceBP(1, 0));
					FFortItemEntry& Entry = BP->ItemEntry;
					Controller->Inventory->Inventory.ReplicatedEntries.Add(Entry);
					Controller->Inventory->Inventory.ItemInstances.Add(BP);
					Controller->Inventory->Inventory.MarkItemDirty(Entry);
					Controller->Inventory->HandleInventoryLocalUpdate();
					// Equip the weapon
					Pawn->EquipWeaponDefinition(WeaponDef, Entry.ItemGuid, FGuid(), false);
				}
			}
			else {
				// Fallback to random
				std::string RandWeapon = Lootpool::Weapons::First[RandomInt(0, Lootpool::Weapons::First.size() - 1)];
				auto WeaponDef = UObject::FindObject<UFortWeaponRangedItemDefinition>(RandWeapon);
				if (WeaponDef) {
					UFortWorldItem* BP = Cast<UFortWorldItem>(WeaponDef->CreateTemporaryItemInstanceBP(1, 0));
					FFortItemEntry& Entry = BP->ItemEntry;
					Controller->Inventory->Inventory.ReplicatedEntries.Add(Entry);
					Controller->Inventory->Inventory.ItemInstances.Add(BP);
					Controller->Inventory->Inventory.MarkItemDirty(Entry);
					Controller->Inventory->HandleInventoryLocalUpdate();
					// Equip the weapon
					Pawn->EquipWeaponDefinition(WeaponDef, Entry.ItemGuid, FGuid(), false);
				}
			}
		}
		else if (EquippedWeaponDef && EquippedEntry) {
			// If already has a ranged weapon, make sure it's equipped
			Pawn->EquipWeaponDefinition(EquippedWeaponDef, EquippedEntry->ItemGuid, FGuid(), false);
		}
		// Find nearest player
		AFortPlayerPawn* nearestPlayer = nullptr;
		float minDist = FLT_MAX;
		FVector bossLoc = Pawn->K2_GetActorLocation();
		for (auto PlayerController : GetGameMode()->AlivePlayers) {
			if (!PlayerController || !PlayerController->MyFortPawn) continue;
			FVector playerLoc = PlayerController->MyFortPawn->K2_GetActorLocation();
			float dx = playerLoc.X - bossLoc.X;
			float dy = playerLoc.Y - bossLoc.Y;
			float dz = playerLoc.Z - bossLoc.Z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist < minDist) {
				minDist = dist;
				nearestPlayer = PlayerController->MyFortPawn;
			}
		}
		// Set AI to move to nearest player
		if (nearestPlayer) {
			Controller->MoveToActor(nearestPlayer, 0.0f, true, true, true, 0, true);
			// Force the AI to treat the player as a hostile target and attack
			Controller->AddTargetInfo(nearestPlayer, true, SDK::EPerceptionState(0), 0.0f, 0.0f);
		}
	}
	// --- BOSS LOGIC END ---

		// Medallion als Boss-Loot droppen
	for (const auto& medPath : MedallionPaths) {
		auto MedallionDef = UObject::FindObject<UFortItemDefinition>(medPath);
		if (MedallionDef) {
			UFortWorldItem* BP = Cast<UFortWorldItem>(MedallionDef->CreateTemporaryItemInstanceBP(1, 0));
			FFortItemEntry& Entry = BP->ItemEntry;
			Controller->Inventory->Inventory.ReplicatedEntries.Add(Entry);
			Controller->Inventory->Inventory.ItemInstances.Add(BP);
			Controller->Inventory->Inventory.MarkItemDirty(Entry);
			Controller->Inventory->HandleInventoryLocalUpdate();
		}
	}

	AIBotInventoryOnSpawnedOG(InventoryBase, Pawn);
}

void (*AIBotGameplayAbilityBaseOnSpawnedOG)(UFortAthenaAISpawnerDataComponent_AIBotGameplayAbilityBase* AIBotGameplayAbilityBase, AFortPlayerPawn* Pawn);
void AIBotGameplayAbilityBaseOnSpawned(UFortAthenaAISpawnerDataComponent_AIBotGameplayAbilityBase* AIBotGameplayAbilityBase, AFortPlayerPawn* Pawn)
{

	auto Controller = (AFortAthenaAIBotController*)Pawn->Controller;

	if (!Controller)
		return;

	auto PlayerState = (AFortPlayerState*)Controller->PlayerState;

	if (!PlayerState)
		return;

	auto AbilitySystemComponent = PlayerState->AbilitySystemComponent;

	if (!AbilitySystemComponent)
		return;

	for (const auto& InitialGameplayAbilitiesSet : AIBotGameplayAbilityBase->InitialGameplayAbilitiesSet)
	{
		GiveAbilitySet((AFortPlayerControllerAthena*)Controller, InitialGameplayAbilitiesSet);
	}

	for (const auto& InitialGameplayEffect : AIBotGameplayAbilityBase->InitialGameplayEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

		AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(InitialGameplayEffect.GameplayEffect, InitialGameplayEffect.Level, EffectContext);
	}
}
void (*AIBotSkillsetOnSpawnedOG)(UFortAthenaAISpawnerDataComponent_AIBotSkillset* AIBotSkillset, AFortPlayerPawn* Pawn);
void AIBotSkillsetOnSpawned(UFortAthenaAISpawnerDataComponent_AIBotSkillset* AIBotSkillset, AFortPlayerPawn* Pawn)
{
	auto AIBotController = (AFortAthenaAIBotController*)Pawn->Controller;
	if (!AIBotController)
		return;

	// Always add these skillsets for bosses
	if (Pawn->GetFullName().find("PaprikaBoss") != std::string::npos) {
		if (AIBotSkillset->AimingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->AimingSkillSet);
		if (AIBotSkillset->AttackingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->AttackingSkillSet);
		if (AIBotSkillset->RangeAttackSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->RangeAttackSkillSet);
	}

	if (AIBotController->BotSkillSetClasses.Num() == 0)
	{
		if (AIBotSkillset->AimingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->AimingSkillSet);

		if (AIBotSkillset->AttackingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->AttackingSkillSet);

		if (AIBotSkillset->BuildingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->BuildingSkillSet);

		if (AIBotSkillset->DBNOSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->DBNOSkillSet);

		if (AIBotSkillset->EmoteSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->EmoteSkillSet);

		if (AIBotSkillset->EvasiveManeuversSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->EvasiveManeuversSkillSet);

		if (AIBotSkillset->HarvestSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->HarvestSkillSet);

		if (AIBotSkillset->HealingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->HealingSkillSet);

		if (AIBotSkillset->InventorySkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->InventorySkillSet);

		if (AIBotSkillset->LootingSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->LootingSkillSet);

		if (AIBotSkillset->MovementSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->MovementSkillSet);

		if (AIBotSkillset->PerceptionSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->PerceptionSkillSet);

		if (AIBotSkillset->PlayStyleSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->PlayStyleSkillSet);

		if (AIBotSkillset->PropagateAwarenessSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->PropagateAwarenessSkillSet);

		if (AIBotSkillset->RangeAttackSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->RangeAttackSkillSet);

		if (AIBotSkillset->ReviveSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->ReviveSkillSet);

		if (AIBotSkillset->UnstuckSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->UnstuckSkillSet);

		if (AIBotSkillset->VehicleSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->VehicleSkillSet);

		if (AIBotSkillset->ReactToVerbSkillSet)
			AIBotController->BotSkillSetClasses.Add(AIBotSkillset->ReactToVerbSkillSet);
		if (auto PlayerBotSkillset = Cast< UFortAthenaAISpawnerDataComponent_PlayerBotSkillset>(AIBotSkillset))
			if (PlayerBotSkillset->WarmUpSkillSet)
				AIBotController->BotSkillSetClasses.Add(PlayerBotSkillset->WarmUpSkillSet);
	}

}

#endif // !AI