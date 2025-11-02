#ifndef INVENTORY
#define INVENTORY
#pragma once
#include "Utils.h"
#include "MedallionRegen.h"
EFortQuickBars GetQuickbar(UFortItemDefinition* ItemDefinition)
{
	if (!ItemDefinition)
		return EFortQuickBars::Max_None;

	return ItemDefinition->ItemType == EFortItemType::WeaponHarvest || ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) || ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) || ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()) || ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) || ((UFortWorldItemDefinition*)ItemDefinition)->bForceIntoOverflow ? EFortQuickBars::Secondary : EFortQuickBars::Primary;
}
bool IsPrimaryQuickbar(UFortItemDefinition* Def)
{
	return GetQuickbar(Def) == EFortQuickBars::Primary;
}

float GetMaxStackSize(UFortItemDefinition* Def)
{
	if (!Def->MaxStackSize.Curve.CurveTable)
		return Def->MaxStackSize.Value;
	EEvaluateCurveTableResult Result;
	float Ret;
	((UDataTableFunctionLibrary*)UDataTableFunctionLibrary::StaticClass()->DefaultObject)->EvaluateCurveTableRow(Def->MaxStackSize.Curve.CurveTable, Def->MaxStackSize.Curve.RowName, 0, &Result, &Ret, FString());
	return Ret;
}

FFortItemEntry* FindEntry(AFortPlayerController* PC, UFortItemDefinition* Def)
{

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}


	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		if (Inventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
			return &Inventory->Inventory.ReplicatedEntries[i];
	}
	return nullptr;
}

FFortItemEntry* FindEntry(AFortPlayerController* PC, FGuid& Guid)
{
	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}


	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		if (Inventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
			return &Inventory->Inventory.ReplicatedEntries[i];
	}
	return nullptr;
}

template<typename T>
FFortItemEntry* FindEntry(AFortPlayerController* PC)
{
	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		if (Inventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(T::StaticClass()))
			return &Inventory->Inventory.ReplicatedEntries[i];
	}
	return nullptr;
}

void UpdateInventory(AFortPlayerControllerAthena* PC, FFortItemEntry* Entry)
{

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	Inventory->bRequiresLocalUpdate = true;
	Inventory->HandleInventoryLocalUpdate();

	if (Entry)
	{
		Inventory->Inventory.MarkItemDirty(*Entry);
	}
	else
	{
		Inventory->Inventory.MarkArrayDirty();
	}
}

void ModifyEntry(AFortPlayerControllerAthena* PC, FFortItemEntry& Entry)
{

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	for (size_t i = 0; i < Inventory->Inventory.ItemInstances.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;

		if (Inventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == Entry.ItemGuid)
		{
			Inventory->Inventory.ItemInstances[i]->ItemEntry = Entry;
			break;
		}
	}
	UpdateInventory(PC, &Entry);
}



UFortWorldItem* GiveItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = 0, int Level = 0)
{
	if (!Def || Count < 1)
		return nullptr;

	UFortWorldItem* Item = Cast<UFortWorldItem>(Def->CreateTemporaryItemInstanceBP(Count, 1));
	Item->SetOwningControllerForTemporaryItem(PC);
	Item->ItemEntry.LoadedAmmo = LoadedAmmo;
	Item->ItemEntry.Count = Count;

	for (auto val : Item->ItemEntry.StateValues) {
		if (val.StateType == EFortItemEntryState::ShouldShowItemToast)
			val.IntValue = 1;
	}


	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	if (!Inventory)
		return nullptr;
	if (auto Weapon = Cast<UFortWeaponRangedItemDefinition>(Def)) {
		Item->ItemEntry.WeaponModSlots = Weapon->WeaponModSlots;
	}
	Inventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
	Inventory->Inventory.ItemInstances.Add(Item);

	((bool(*)(UFortItemDefinition*, void*, UFortWorldItem*, uint8)) (*(void***)Def)[0x8b])(Def, GetInterfaceAddress(PC, IFortInventoryOwnerInterface::StaticClass()), Item, 1);
	UpdateInventory(PC, &Item->ItemEntry);

	// Medallion-Check und Schild-Regeneration starten
	if (IsMedallionItem(Def)) {
		StartMedallionRegen(PC);
	}

	return Item;
}

void GiveItemWithMods(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def, AFortPickup* Pickup, int Count = 1, int LoadedAmmo = 0, int Level = 0) {
	if (!Def || Count < 1)
		return;

	UFortWorldItem* Item = Cast<UFortWorldItem>(Def->CreateTemporaryItemInstanceBP(Count, 1));
	Item->SetOwningControllerForTemporaryItem(PC);
	Item->ItemEntry.LoadedAmmo = LoadedAmmo;
	Item->ItemEntry.Count = Count;

	for (auto val : Item->ItemEntry.StateValues) {
		if (val.StateType == EFortItemEntryState::ShouldShowItemToast)
			val.IntValue = 1;
	}

	Item->ItemEntry.WeaponModSlots.Free();
	Item->ItemEntry.WeaponModSlots = Pickup->PrimaryPickupItemEntry.WeaponModSlots;


	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}
	if (!Inventory->Inventory.ReplicatedEntries.IsValid())
		return;

	Inventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
	Inventory->Inventory.ItemInstances.Add(Item);
	((bool(*)(UFortItemDefinition*, void*, UFortWorldItem*, uint8)) (*(void***)Def)[0x8b])(Def, GetInterfaceAddress(PC, IFortInventoryOwnerInterface::StaticClass()), Item, 1);
	UpdateInventory(PC, &Item->ItemEntry);
}

void RemoveItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def, int Count)
{

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}
	if (!Inventory->Inventory.ReplicatedEntries.IsValid())
		return;

	bool Remove = false;
	FGuid guid;
	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		auto& Entry = Inventory->Inventory.ReplicatedEntries[i];
		if (Entry.ItemDefinition == Def)
		{
			Entry.Count -= Count;
			if (Entry.Count <= 0)
			{
				Inventory->Inventory.ReplicatedEntries[i].StateValues.Free();
				Inventory->Inventory.ReplicatedEntries.Remove(i);
				Remove = true;
				guid = Entry.ItemGuid;
			}
			else
			{
				Inventory->Inventory.MarkItemDirty(Inventory->Inventory.ReplicatedEntries[i]);
				UpdateInventory(PC, &Entry);
				Inventory->HandleInventoryLocalUpdate();
				return;
			}
			break;
		}
	}

	if (Remove)
	{
		for (size_t i = 0; i < Inventory->Inventory.ItemInstances.Num(); i++)
		{
			if (!Inventory->Inventory.ItemInstances.IsValidIndex(i))
				continue;
			if (Inventory->Inventory.ItemInstances[i]->GetItemGuid() == guid)
			{
				auto Instance = Inventory->Inventory.ItemInstances[i];
				Inventory->Inventory.ItemInstances.Remove(i);
				((bool(*)(UItemDefinitionBase*, void*, UFortWorldItem*)) (*(void***)Def)[0x8c])(Def, GetInterfaceAddress(PC, IFortInventoryOwnerInterface::StaticClass()), Instance);
				break;
			}
		}
	}


	Inventory->Inventory.MarkArrayDirty();
	Inventory->HandleInventoryLocalUpdate();
}
void RemoveItem(AFortPlayerControllerAthena* PC, FGuid Guid, int Count)
{

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	for (size_t i = 0; Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;

		auto Entry = Inventory->Inventory.ReplicatedEntries[i];
		if (Entry.ItemGuid == Guid)
		{
			RemoveItem(PC, (UFortItemDefinition*)Entry.ItemDefinition, Count);
			break;
		}
	}
}

void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon)
{
	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}
	if (!Inventory->Inventory.ReplicatedEntries.IsValid())
		return;

	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;

		if (Inventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
		{
			Inventory->Inventory.ReplicatedEntries[i].LoadedAmmo = Weapon->AmmoCount;
			ModifyEntry((AFortPlayerControllerAthena*)PC, Inventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &Inventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}

void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon, int AmountToAdd)
{
	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!PC->WorldInventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo += AmountToAdd;
			ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}


void GiveStartingItems(AFortPlayerControllerAthena* PlayerController)
{
	if (!PlayerController)
		return;

	AFortGameModeAthena* GameMode = Cast<AFortGameModeAthena>(UWorld::GetWorld()->AuthorityGameMode);
	if (!GameMode)
		return;

	bool bHasStartingItems = false;

	for (const FFortItemEntry& ItemEntry : PlayerController->WorldInventory->Inventory.ReplicatedEntries)
	{
		if (ItemEntry.ItemDefinition && ItemEntry.ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
			bHasStartingItems = true;
	}

	if (!bHasStartingItems)
	{
		for (const FItemAndCount& Item : GameMode->StartingItems)
		{
			if (!Item.Item || (Item.Item && Item.Item->IsA(UFortResourceItemDefinition::StaticClass())))
				continue;

			if (Item.Item->IsA(UFortSmartBuildingItemDefinition::StaticClass()))
				continue;
			if (!Item.Item->IsA(UFortItemDefinition::StaticClass()))
				continue;
			GiveItem(PlayerController, (UFortItemDefinition*)Item.Item, Item.Count, 0);
		}

		UFortWeaponMeleeItemDefinition* LoadoutPickaxeDef = PlayerController->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;
		if (LoadoutPickaxeDef)
			GiveItem(PlayerController, LoadoutPickaxeDef, 1, 0);
	}
	//UFortKismetLibrary::K2_RemoveItemFromPlayerByGuid

}
//int32 UFortKismetLibrary::K2_RemoveItemFromPlayerByGuid(class AFortPlayerController* PlayerController, const struct FGuid& ItemGuid, int32 AmountToRemove, bool bForceRemoval)

int (*K2_RemoveItemFromPlayerByGuidOG)(UObject* Context, FFrame& Params);
int K2_RemoveItemFromPlayerByGuid(UObject* Context, FFrame& Params) {
	AFortPlayerControllerAthena* PlayerController;
	int AmountToRemove;
	bool ForceRemove;

	Params.StepCompiledIn(&PlayerController);
	auto& ItemGuid = Params.StepCompiledInRef<FGuid>();
	Params.StepCompiledIn(&AmountToRemove);
	Params.StepCompiledIn(&ForceRemove);

	if (!PlayerController)
		return K2_RemoveItemFromPlayerByGuidOG(Context, Params);

	RemoveItem(PlayerController, ItemGuid, AmountToRemove);
	return K2_RemoveItemFromPlayerByGuidOG(Context, Params);
}

int (*K2_RemoveItemFromPlayerOG)(UObject* Context, FFrame& Params);
int K2_RemoveItemFromPlayer(UObject* Context, FFrame& Params) {
	AFortPlayerControllerAthena* Controller = nullptr;
	UFortWorldItemDefinition* ItemDefinition = nullptr;
	int AmountToRemove;
	Params.StepCompiledIn(&Controller);
	Params.StepCompiledIn(&ItemDefinition);
	auto& ItemVariantGuid = Params.StepCompiledInRef<FGuid>();
	Params.StepCompiledIn(&AmountToRemove);

	if (!Controller || !ItemDefinition)
		return K2_RemoveItemFromPlayerOG(Context, Params);

	RemoveItem(Controller, ItemDefinition, AmountToRemove);
	return K2_RemoveItemFromPlayerOG(Context, Params);

}
int (*K2_RemoveFortItemFromPlayerOG)(UObject* Context, FFrame& Params);
int K2_RemoveFortItemFromPlayer(UObject* Context, FFrame& Params) {
	AFortPlayerControllerAthena* Controller;
	UFortItem* Item;
	int AmountToRemove;
	bool bFroceRemove;
	Params.StepCompiledIn(&Controller);
	Params.StepCompiledIn(&Item);
	Params.StepCompiledIn(&AmountToRemove);
	Params.StepCompiledIn(&bFroceRemove);

	if (!Controller || !Item)
		return K2_RemoveFortItemFromPlayerOG(Context, Params);

	RemoveItem(Controller, Item->GetItemGuid(), AmountToRemove);
	return K2_RemoveFortItemFromPlayerOG(Context, Params);
}


#endif // !INVENTORY
