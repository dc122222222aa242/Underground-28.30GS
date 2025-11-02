#ifndef PAWN
#define PAWN
#pragma once
#include "Utils.h"
#include "Inventory.h"

void ServerHandlePickup(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
{
	if (!Pawn)
		return;

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->GetController();
	if (!PC)
		return;

	if (!Pickup || Pickup->bPickedUp || Pawn->GetController()->IsA(AFortAthenaAIBotController::StaticClass()))
		return;

	UFortItemDefinition* Def = (UFortItemDefinition*)Pickup->PrimaryPickupItemEntry.ItemDefinition;
	
	int PickupCount = Pickup->PrimaryPickupItemEntry.Count;
	int PickupLoadedAmmo = Pickup->PrimaryPickupItemEntry.LoadedAmmo;
	FFortItemEntry* FoundEntry = nullptr;
	int Items = 0;
	float MaxStackSize = GetMaxStackSize(Def);
	bool Stackable = Def->IsStackable();

	if (!Pawn->CurrentWeapon)
		return;

	if (!Pawn->CurrentWeapon->WeaponData)
		return;

	auto Inventory = PC->WorldInventory;
	if (!Inventory && PC->IsA(AFortAthenaAIBotController::StaticClass()))
	{
		Inventory = ((AFortAthenaAIBotController*)PC)->Inventory;
	}

	if (!Inventory)
		return;

	if (!Inventory->Inventory.ReplicatedEntries.IsValid())
		return;

	for (size_t i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (!Inventory->Inventory.ReplicatedEntries.IsValidIndex(i))
			continue;
		
		FFortItemEntry& Entry = Inventory->Inventory.ReplicatedEntries[i];

		if (IsPrimaryQuickbar(Def) && IsPrimaryQuickbar((UFortItemDefinition*)Entry.ItemDefinition))
		{
			Items++;
		}

		if (Entry.ItemDefinition == Def && (Entry.Count < MaxStackSize))
		{
			FoundEntry = &Inventory->Inventory.ReplicatedEntries[i];
		}
	}

	
	if (IsPrimaryQuickbar(Def) && Items >= 6)
	{
		
		FFortItemEntry* SwapEntry = FindEntry(PC, Pawn->CurrentWeapon->WeaponData);
		if (SwapEntry && SwapEntry->ItemDefinition && SwapEntry->ItemDefinition->IsA(UFortWeaponItemDefinition::StaticClass()))
		{
			SpawnPickup(Pawn->K2_GetActorLocation(), (UFortItemDefinition*)SwapEntry->ItemDefinition, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, SwapEntry->Count, SwapEntry->LoadedAmmo);
			RemoveItem(PC, Pawn->CurrentWeapon->GetInventoryGUID(), SwapEntry->Count);
			
		}
	}

	if (FoundEntry)
	{
		if (Stackable)
		{
			FoundEntry->Count += PickupCount;
			if (FoundEntry->Count > MaxStackSize)
			{
				int Count = FoundEntry->Count;
				FoundEntry->Count = MaxStackSize;

				if (Def->bAllowMultipleStacks)
				{
					if (Items == 5)
					{
						SpawnPickup(Pawn->K2_GetActorLocation(), (UFortItemDefinition*)FoundEntry->ItemDefinition, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Count - MaxStackSize, FoundEntry->LoadedAmmo);
					}
					else
					{

						GiveItem(PC, Def, Count - MaxStackSize, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
					}
				}
				else
				{
					SpawnPickup(Pawn->K2_GetActorLocation(), (UFortItemDefinition*)FoundEntry->ItemDefinition, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Count - MaxStackSize, FoundEntry->LoadedAmmo);
				}
			}

			UpdateInventory(PC, FoundEntry);
		}
		else if (Def->bAllowMultipleStacks)
		{
			if (Def->IsA(UFortWeaponItemDefinition::StaticClass()))
				GiveItemWithMods(PC, Def, Pickup, PickupCount, PickupLoadedAmmo, 0);
			else {
				GiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
			}
		}
	}
	else
	{
		if (Def->IsA(UFortWeaponItemDefinition::StaticClass()))
			GiveItemWithMods(PC, Def, Pickup, PickupCount, PickupLoadedAmmo, 0);
		else {
			GiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
		}
	}
	
	Pickup->PickupLocationData.bPlayPickupSound = bPlayPickupSound;
	Pickup->PickupLocationData.FlyTime = 0.4f;
	Pickup->PickupLocationData.ItemOwner = Pawn;
	Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
	Pickup->PickupLocationData.PickupTarget = Pawn;
	Pickup->OnRep_PickupLocationData();

	Pickup->bPickedUp = true;
	Pickup->OnRep_bPickedUp();
}

void (*OnCapsuleBeginOverlapOG)(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult SweepResult);
void OnCapsuleBeginOverlap(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult SweepResult)
{
	if (OtherActor->IsA(AFortPickup::StaticClass()))
	{
		AFortPickup* Pickup = (AFortPickup*)OtherActor;

		if (Pickup->PawnWhoDroppedPickup == Pawn)
			return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		UFortItemDefinition* Def = (UFortItemDefinition*)Pickup->PrimaryPickupItemEntry.ItemDefinition;

		if (!IsPrimaryQuickbar(Def))
		{
			ServerHandlePickup(Pawn, Pickup, 0.40f, FVector(), true);
		}
	}

	return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ServerHandlePickupWithSwap(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, FGuid Swap, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
{
	if (!Pawn || !Pickup || Pickup->bPickedUp)
		return;

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;

	FFortItemEntry* Entry = FindEntry(PC, Swap);
	if (!Entry || !Entry->ItemDefinition || Entry->ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
		return;
	
	SpawnPickup(Pawn->K2_GetActorLocation(), (UFortItemDefinition*)Entry->ItemDefinition, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, 1, Entry->LoadedAmmo);
	RemoveItem(PC, Swap, Entry->Count);
	ServerHandlePickup(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);
}

void (*ServerHandlePickupInfoOG)(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, const FFortPickupRequestInfo& Params);
void ServerHandlePickupInfo(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, const FFortPickupRequestInfo& Params) {
	if (Params.bUseRequestedSwap) {
		return ServerHandlePickupWithSwap(Pawn, Pickup, Params.SwapWithItem, 0.40f, FVector(), Params.bPlayPickupSound);
	}
	else 
		return ServerHandlePickup(Pawn, Pickup, 0.40f, FVector(), Params.bPlayPickupSound);
}
#endif // PAWN


