#ifndef LOOTING
#define LOOTING
#pragma once
#include "Utils.h"
#include "Inventory.h"
#include <map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <string>
// full skidded from reboot lol
int GetItemLevel(const FDataTableCategoryHandle& LootLevelData, int WorldLevel)
{
	auto DataTable = LootLevelData.DataTable;

	if (!DataTable)
		return 0;

	if (!LootLevelData.ColumnName.ComparisonIndex)
		return 0;

	if (!LootLevelData.RowContents.ComparisonIndex)
		return 0;

	std::vector<FFortLootLevelData*> OurLootLevelDatas;

	for (auto& LootLevelDataPair : LootLevelData.DataTable->RowMap)
	{
		if (((FFortLootLevelData*)(LootLevelDataPair.Second))->Category != LootLevelData.RowContents)
			continue;

		OurLootLevelDatas.push_back(((FFortLootLevelData*)(LootLevelDataPair.Second)));
	}

	if (OurLootLevelDatas.size() > 0)
	{
		int PickedIndex = -1;
		int PickedLootLevel = 0;

		for (int i = 0; i < OurLootLevelDatas.size(); i++)
		{
			auto CurrentLootLevelData = OurLootLevelDatas.at(i);

			if (CurrentLootLevelData->LootLevel <= WorldLevel && CurrentLootLevelData->LootLevel > PickedLootLevel)
			{
				PickedLootLevel = CurrentLootLevelData->LootLevel;
				PickedIndex = i;
			}
		}

		if (PickedIndex != -1)
		{
			auto PickedLootLevelData = OurLootLevelDatas.at(PickedIndex);

			const auto PickedMinItemLevel = PickedLootLevelData->MinItemLevel;
			const auto PickedMaxItemLevel = PickedLootLevelData->MaxItemLevel;
			auto v15 = PickedMaxItemLevel - PickedMinItemLevel;

			if (v15 + 1 <= 0)
			{
				v15 = 0;
			}
			else
			{
				auto v16 = (int)(float)((float)((float)rand() * 0.000030518509) * (float)(v15 + 1));
				if (v16 <= v15)
					v15 = v16;
			}

			return v15 + PickedMinItemLevel;
		}
	}

	return 0;
}
int PickLevel(UFortWorldItemDefinition* def, int PreferredLevel)
{

	const int MinLevel = def->MinLevel;
	const int MaxLevel = def->MaxLevel;

	int PickedLevel = 0;

	if (PreferredLevel >= MinLevel)
		PickedLevel = PreferredLevel;

	if (MaxLevel >= 0)
	{
		if (PickedLevel <= MaxLevel)
			return PickedLevel;
		return MaxLevel;
	}

	return PickedLevel;
}
#define LOOTING_MAP_TYPE std::map
static inline float RandomFloatForLoot(float AllWeightsSum)
{
	return (rand() * 0.000030518509f) * AllWeightsSum;
}

template <typename KeyType, typename ValueType>
FORCEINLINE static ValueType PickWeightedElement(const std::map<KeyType, ValueType>& Elements,
	std::function<float(ValueType)> GetWeightFn,
	std::function<float(float)> RandomFloatGenerator = RandomFloatForLoot,
	float TotalWeightParam = -1, bool bCheckIfWeightIsZero = false, int RandMultiplier = 1, KeyType* OutName = nullptr, bool bPrint = false, bool bKeepGoingUntilWeGetValue = false)
{
	float TotalWeight = TotalWeightParam;

	if (TotalWeight == -1)
	{
		TotalWeight = std::accumulate(Elements.begin(), Elements.end(), 0.0f, [&](float acc, const std::pair<KeyType, ValueType>& p) {
			auto Weight = GetWeightFn(p.second);

			return acc + Weight;
			});
	}

	float RandomNumber =  
		RandMultiplier * RandomFloatGenerator(TotalWeight);


	for (auto& Element : Elements)
	{
		float Weight = GetWeightFn(Element.second);

		if (bCheckIfWeightIsZero && Weight == 0)
			continue;

		if (RandomNumber <= Weight)
		{
			if (OutName)
				*OutName = Element.first;

			return Element.second;
		}

		RandomNumber -= Weight;
	}

	if (bKeepGoingUntilWeGetValue)
		return PickWeightedElement<KeyType, ValueType>(Elements, GetWeightFn, RandomFloatGenerator, TotalWeightParam, bCheckIfWeightIsZero, RandMultiplier, OutName, bPrint, bKeepGoingUntilWeGetValue);

	return ValueType();
}

template <typename RowStructType = uint8>
void CollectDataTablesRows(const std::vector<UDataTable*>& DataTables, LOOTING_MAP_TYPE<FName, RowStructType*>* OutMap, std::function<bool(FName, RowStructType*)> Check = []() { return true; })
{
	std::vector<UDataTable*> DataTablesToIterate;

	for (UDataTable* DataTable : DataTables)
	{
		if (!DataTable || IsBadReadPtr(DataTable, 8) || !DataTable->Class)
		{
			continue;
		}
		if (auto CompositeDataTable = Cast<UCompositeDataTable>(DataTable))
		{
			static auto ParentTables = CompositeDataTable->ParentTables;

			for (int i = 0; i < ParentTables.Num(); ++i)
			{
				DataTablesToIterate.push_back(ParentTables[i]);
			}
		}

		DataTablesToIterate.push_back(DataTable);
	}

	for (auto CurrentDataTable : DataTablesToIterate)
	{
		for (TPair<FName, uint8_t*>& CurrentPair : CurrentDataTable->RowMap)
		{
			if (Check(CurrentPair.Key(), (RowStructType*)CurrentPair.Value()))
			{
				OutMap->insert_or_assign(CurrentPair.Key(), (RowStructType*)CurrentPair.Value());
			}
		}
	}
}
FFortLootTierData* PickLootTierData(const std::vector<UDataTable*>& LTDTables, FName LootTierGroup, int ForcedLootTier = -1, FName* OutRowName = nullptr) // Fortnite returns the row name and then finds the tier data again, but I really don't see the point of this.
{
	float LootTier = ForcedLootTier;

	int Multiplier = LootTier == -1 ? 1 : LootTier;

	LOOTING_MAP_TYPE<FName, FFortLootTierData*> TierGroupLTDs;

	CollectDataTablesRows<FFortLootTierData>(LTDTables, &TierGroupLTDs, [&](FName RowName, FFortLootTierData* TierData) -> bool {
		if (TierData->TierGroup.ComparisonIndex == LootTierGroup.ComparisonIndex)
		{
			return true;
		}

		return false;
		});

	FFortLootTierData* ChosenRowLootTierData = PickWeightedElement<FName, FFortLootTierData*>(TierGroupLTDs,
		[](FFortLootTierData* LootTierData) -> float { return LootTierData->Weight; }, RandomFloatForLoot, -1,
		true, Multiplier, OutRowName);

	return ChosenRowLootTierData;
}
float GetAmountOfLootPackagesToDrop(FFortLootTierData* LootTierData, int OriginalNumberLootDrops)
{
	if (LootTierData->LootPackageCategoryMinArray.Num() != LootTierData->LootPackageCategoryWeightArray.Num()
		|| LootTierData->LootPackageCategoryMinArray.Num() != LootTierData->LootPackageCategoryWeightArray.Num()
		)
		return 0;


	float MinimumLootDrops = 0;

	if (LootTierData->LootPackageCategoryMinArray.Num() > 0)
	{
		for (int i = 0; i < LootTierData->LootPackageCategoryMinArray.Num(); ++i)
		{
			MinimumLootDrops += LootTierData->LootPackageCategoryMinArray[i];
		}
	}

	int SumLootPackageCategoryWeightArray = 0;

	if (LootTierData->LootPackageCategoryWeightArray.Num() > 0)
	{
		for (int i = 0; i < LootTierData->LootPackageCategoryWeightArray.Num(); ++i)
		{
			if (LootTierData->LootPackageCategoryWeightArray[i] > 0)
			{
				auto LootPackageCategoryMaxArrayIt = LootTierData->LootPackageCategoryMaxArray[i];

				float IDK = 0;

				if (LootPackageCategoryMaxArrayIt < 0 || IDK < LootPackageCategoryMaxArrayIt)
				{
					SumLootPackageCategoryWeightArray += LootTierData->LootPackageCategoryWeightArray[i];
				}
			}
		}
	}

	while (SumLootPackageCategoryWeightArray > 0)
	{
		float v29 = (float)rand() * 0.000030518509f;

		float v35 = (int)(float)((float)((float)((float)SumLootPackageCategoryWeightArray * v29)
			+ (float)((float)SumLootPackageCategoryWeightArray * v29))
			+ 0.5f) >> 1;

		MinimumLootDrops++;

		if (MinimumLootDrops >= OriginalNumberLootDrops)
			return MinimumLootDrops;

		SumLootPackageCategoryWeightArray--;
	}

	return MinimumLootDrops;
}
void PickLootDropsFromLootPackage(const std::vector<UDataTable*>& LPTables, const FName& LootPackageName, std::vector<FFortItemEntry>* OutEntries, int LootPackageCategory = -1, int WorldLevel = 0, bool bPrint = false, bool bCombineDrops = true)
{
	if (!OutEntries)
		return;

	LOOTING_MAP_TYPE<FName, FFortLootPackageData*> LootPackageIDMap;

	CollectDataTablesRows<FFortLootPackageData>(LPTables, &LootPackageIDMap, [&](FName RowName, FFortLootPackageData* LootPackage) -> bool {
		if (LootPackage->LootPackageID != LootPackageName)
		{
			return false;
		}

		if (LootPackageCategory != -1 && LootPackage->LootPackageCategory != LootPackageCategory) // idk if proper
		{
			return false;
		}

		if (WorldLevel >= 0)
		{
			if (LootPackage->MaxWorldLevel >= 0 && WorldLevel > LootPackage->MaxWorldLevel)
				return 0;

			if (LootPackage->MinWorldLevel >= 0 && WorldLevel < LootPackage->MinWorldLevel)
				return 0;
		}

		return true;
		});

	if (LootPackageIDMap.size() == 0)
	{
		return;
	}

	FName PickedPackageRowName;

	FFortLootPackageData* PickedPackage = PickWeightedElement<FName, FFortLootPackageData*>(LootPackageIDMap,
		[](FFortLootPackageData* LootPackageData) -> float { return LootPackageData->Weight; }, RandomFloatForLoot,
		-1, true, 1, &PickedPackageRowName, bPrint);

	if (!PickedPackage)
		return;

	if (PickedPackage->LootPackageCall.Num() > 1)
	{
		if (PickedPackage->Count > 0)
		{
			int v9 = 0;

			while (v9 < PickedPackage->Count)
			{
				int LootPackageCategoryToUseForLPCall = 0; // hmm

				PickLootDropsFromLootPackage(LPTables,
					PickedPackage->LootPackageCall.Data ? UKismetStringLibrary::Conv_StringToName(PickedPackage->LootPackageCall) : FName(0),
					OutEntries, LootPackageCategoryToUseForLPCall, WorldLevel, bPrint
				);

				v9++;
			}
		}

		return;
	}
	auto ItemDefinition = StaticLoadObject<UFortItemDefinition>(PickedPackage->ItemDefinition.ObjectID.GetAssetPathName());
	if (!ItemDefinition)
	{
		return;
	}
	auto WeaponItemDefinition = Cast<UFortWeaponItemDefinition>(ItemDefinition);
	int LoadedAmmo = WeaponItemDefinition ? GetStats(WeaponItemDefinition)->ClipSize : 0;

	auto WorldItemDefinition = Cast<UFortWorldItemDefinition>(ItemDefinition);

	if (!WorldItemDefinition)
		return;

	int ItemLevel = GetItemLevel(WorldItemDefinition->LootLevelData, WorldLevel);

	int CountMultiplier = 1;
	int FinalCount = CountMultiplier * PickedPackage->Count;

	if (FinalCount > 0)
	{
		int FinalItemLevel = 0;

		if (ItemLevel >= 0)
			FinalItemLevel = ItemLevel;

		while (FinalCount > 0)
		{
			int MaxStackSize = GetMaxStackSize(ItemDefinition);

			int CurrentCountForEntry = MaxStackSize;

			if (FinalCount <= MaxStackSize)
				CurrentCountForEntry = FinalCount;

			if (CurrentCountForEntry <= 0)
				CurrentCountForEntry = 0;

			auto ActualItemLevel = PickLevel(WorldItemDefinition, FinalItemLevel);

			bool bHasCombined = false;

			if (bCombineDrops)
			{
				for (auto& CurrentLootDrop : *OutEntries)
				{
					if (CurrentLootDrop.ItemDefinition == ItemDefinition)
					{
						int NewCount = CurrentLootDrop.Count + CurrentCountForEntry;

						if (NewCount <= GetMaxStackSize(ItemDefinition))
						{
							bHasCombined = true;
							CurrentLootDrop.Count = NewCount;
						}
					}
				}
			}

			if (!bHasCombined)
			{
				OutEntries->push_back(*MakeItemEntry(ItemDefinition, CurrentCountForEntry, ActualItemLevel));
			}

			bool IsWeapon = PickedPackage->LootPackageID.ToString().contains(".Weapon.") && WeaponItemDefinition; 

			if (IsWeapon && WeaponItemDefinition->GetAmmoWorldItemDefinition_BP() && WeaponItemDefinition->GetAmmoWorldItemDefinition_BP() != WeaponItemDefinition)
			{
				OutEntries->push_back(*MakeItemEntry(WeaponItemDefinition->GetAmmoWorldItemDefinition_BP(), ((UFortAmmoItemDefinition*)WeaponItemDefinition->GetAmmoWorldItemDefinition_BP())->DropCount, 0));
			}

			FinalCount -= CurrentCountForEntry;
		}
	}
}


std::vector<FFortItemEntry> PickLootDrops(FName TierGroupName, int32 WorldLevel, int32 ForcedLootTier=-1)
{
	std::vector<FFortItemEntry> LootDrops{};

	static std::vector<UDataTable*> LootPackage_Tables;
	static std::vector<UDataTable*> LootTierData_Tables;
	static bool bSetup = false;
	if (!bSetup)
	{
		bSetup = true;
		
		auto Playlist = Cast<AFortGameStateAthena>(UWorld::GetWorld()->GameState)->CurrentPlaylistInfo.BasePlaylist;
		LootTierData_Tables.push_back(StaticLoadObject<UDataTable>(Playlist->LootTierData.ObjectID.GetAssetPathName()));
		LootPackage_Tables.push_back(StaticLoadObject<UDataTable>(Playlist->LootPackages.ObjectID.GetAssetPathName()));

		for (auto i = 0; i < UObject::GObjects->Num(); ++i)
		{
			auto Object = UObject::GObjects->GetByIndex(i);

			if (auto GameFeatureData = Cast<UFortGameFeatureData>(Object)) {
				
				auto DefaultLootTableData = GameFeatureData->DefaultLootTableData;
				auto LootTierDataPtr = StaticLoadObject<UDataTable>(DefaultLootTableData.LootTierData.ObjectID.GetAssetPathName());
				auto LootPackagePtr = StaticLoadObject<UDataTable>(DefaultLootTableData.LootPackageData.ObjectID.GetAssetPathName());

				if (LootPackagePtr)
					LootPackage_Tables.push_back(LootPackagePtr);
				if (Playlist) {
					for (int i = 0; i < Playlist->GameplayTagContainer.GameplayTags.Num(); ++i) {
						auto Tag = Playlist->GameplayTagContainer.GameplayTags[i];

						for (auto Value : GameFeatureData->PlaylistOverrideLootTableData) {
							auto CurrentOverrideTag = Value.First;

							if (Tag.TagName == CurrentOverrideTag.TagName) {
								auto ptr = StaticLoadObject<UDataTable>(Value.Second.LootPackageData.ObjectID.GetAssetPathName());

								if (ptr) {
									LootPackage_Tables.push_back(ptr);
								}
							}
						}
					}
				}
				
				if (LootTierDataPtr)
				{
					LootTierData_Tables.push_back(LootTierDataPtr);
				}

				if (Playlist) {
					for (int i = 0; i < Playlist->GameplayTagContainer.GameplayTags.Num(); ++i) {
						auto Tag = Playlist->GameplayTagContainer.GameplayTags[i];

						for (auto Value : GameFeatureData->PlaylistOverrideLootTableData) {
							auto CurrentOverrideTag = Value.First;

							if (Tag.TagName == CurrentOverrideTag.TagName) {
								auto ptr = StaticLoadObject<UDataTable>(Value.Second.LootPackageData.ObjectID.GetAssetPathName());

								if (ptr) {
									LootPackage_Tables.push_back(ptr);
								}
							}
						}
					}
				}
			}
		}
	}
	FName LootTierRowName;
	auto ChosenRowLootTierData = PickLootTierData(LootTierData_Tables, TierGroupName, ForcedLootTier, &LootTierRowName);
	if (!ChosenRowLootTierData)
	{
		std::cout << "Cant Find LootTierData for: " << TierGroupName.ToString() << "\n";
		return LootDrops;

	}
	float NumLootPackageDrops = ChosenRowLootTierData->NumLootPackageDrops;
	float NumberLootDrops = 0;

	if (NumLootPackageDrops > 0)
	{
		if (NumLootPackageDrops < 1)
		{
			NumberLootDrops = 1;
		}
		else
		{
			NumberLootDrops = (int)(float)((float)(NumLootPackageDrops + NumLootPackageDrops) - 0.5f) >> 1;
			float v20 = NumLootPackageDrops - NumberLootDrops;
			if (v20 > 0.0000099999997f)
			{
				NumberLootDrops += v20 >= (rand() * 0.000030518509f);
			}
		}
	}

	float AmountOfLootPackageDrops = GetAmountOfLootPackagesToDrop(ChosenRowLootTierData, NumberLootDrops);
	LootDrops.reserve(AmountOfLootPackageDrops);
	if (AmountOfLootPackageDrops > 0)
	{
		for (int i = 0; i < AmountOfLootPackageDrops; ++i)
		{
			if (i >= ChosenRowLootTierData->LootPackageCategoryMinArray.Num())
				break;

			for (int j = 0; j < ChosenRowLootTierData->LootPackageCategoryMinArray[i]; ++j)
			{
				if (ChosenRowLootTierData->LootPackageCategoryMinArray[i] < 1)
					break;

				int LootPackageCategory = i;

				PickLootDropsFromLootPackage(LootPackage_Tables, ChosenRowLootTierData->LootPackage, &LootDrops, LootPackageCategory, WorldLevel, false);
			}
		}
	}

	return LootDrops;

}
bool PickLootDropsHook(UObject* Object, FFrame& Stack, bool* Ret)
{
	UObject* WorldContextObject;
	FName TierGroupName;
	int32 WorldLevel;
	int32 ForcedLootTier;
	FGameplayTagContainer Container;
	Stack.StepCompiledIn(&WorldContextObject);
	auto& OutLootToDrop = Stack.StepCompiledInRef<TArray<FFortItemEntry>>();
	Stack.StepCompiledIn(&TierGroupName);
	Stack.StepCompiledIn(&WorldLevel);
	Stack.StepCompiledIn(&ForcedLootTier);
	Stack.StepCompiledIn(&Container);
	Stack.IncrementCode();
	auto LootDrops = PickLootDrops(TierGroupName, ForcedLootTier, WorldLevel);
	for (auto LootDrop : LootDrops)
		OutLootToDrop.Add(LootDrop);
	return *Ret = true;
}
AFortPickup* K2_SpawnPickupInWorldWithClassHook(UObject* Context, FFrame& Stack, AFortPickup** Ret)
{
	UObject* WorldContextObject;                                      
	UFortWorldItemDefinition* ItemDefinition;          
	TSubclassOf<UClass> PickupClass; 
	int	NumberToSpawn;
	FVector  Position;
	FVector Direction; 
	int  OverrideMaxStackCount;
	bool bToss;       
	bool bRandomRotation;
	bool bBlockedFromAutoPickup;
	int PickupInstigatorHandle;
	EFortPickupSourceTypeFlag SourceType;
	EFortPickupSpawnSource Source;
	AFortPlayerController* OptionalOwnerPC;
	bool bPickupOnlyRelevantToOwner;

	Stack.StepCompiledIn(&WorldContextObject);
	Stack.StepCompiledIn(&ItemDefinition);
	Stack.StepCompiledIn(&PickupClass);
	Stack.StepCompiledIn(&NumberToSpawn);
	Stack.StepCompiledIn(&Position);
	Stack.StepCompiledIn(&Direction);
	Stack.StepCompiledIn(&OverrideMaxStackCount);
	Stack.StepCompiledIn(&bToss);
	Stack.StepCompiledIn(&bRandomRotation);
	Stack.StepCompiledIn(&bBlockedFromAutoPickup);
	Stack.StepCompiledIn(&PickupInstigatorHandle);
	Stack.StepCompiledIn(&SourceType);
	Stack.StepCompiledIn(&Source);
	Stack.StepCompiledIn(&OptionalOwnerPC);
	Stack.StepCompiledIn(&bPickupOnlyRelevantToOwner);

	auto GameState = GetGameState();

	auto NewPickup = SpawnPickup(Position, ItemDefinition, SourceType, Source, NumberToSpawn, 0, nullptr);

	*Ret = NewPickup;
	return *Ret;
}
AFortPickup* K2_SpawnPickupInWorldHook(UObject* Object, FFrame& Stack, AFortPickup** Ret)
{
	class UObject* WorldContextObject;
	class UFortWorldItemDefinition* ItemDefinition;
	int32 NumberToSpawn;
	FVector Position;
	FVector Direction;
	int32 OverrideMaxStackCount;
	bool bToss;
	bool bRandomRotation;
	bool bBlockedFromAutoPickup;
	int32 PickupInstigatorHandle;
	EFortPickupSourceTypeFlag SourceType;
	EFortPickupSpawnSource Source;
	class AFortPlayerController* OptionalOwnerPC;
	bool bPickupOnlyRelevantToOwner;
	Stack.StepCompiledIn(&WorldContextObject);
	Stack.StepCompiledIn(&ItemDefinition);
	Stack.StepCompiledIn(&NumberToSpawn);
	Stack.StepCompiledIn(&Position);
	Stack.StepCompiledIn(&Direction);
	Stack.StepCompiledIn(&OverrideMaxStackCount);
	Stack.StepCompiledIn(&bToss);
	Stack.StepCompiledIn(&bRandomRotation);
	Stack.StepCompiledIn(&bBlockedFromAutoPickup);
	Stack.StepCompiledIn(&PickupInstigatorHandle);
	Stack.StepCompiledIn(&SourceType);
	Stack.StepCompiledIn(&Source);
	Stack.StepCompiledIn(&OptionalOwnerPC);
	Stack.StepCompiledIn(&bPickupOnlyRelevantToOwner);
	Stack.IncrementCode();
	return *Ret = SpawnPickup(Position, ItemDefinition, SourceType, Source, NumberToSpawn, 0, OptionalOwnerPC ? OptionalOwnerPC->MyFortPawn:nullptr);
}

AFortPickup* K2_SpawnPickupInWorldWithClassAndItemEntry(UObject* Context, FFrame& Stack, AFortPickup** Ret)
{
	UObject* WorldContextObject;
	FFortItemEntry Entry;
	TSubclassOf<AFortPickup> PickupClass;
	FVector Position;
	FVector Direction;
	int32 OverrideMaxStackCount;
	bool bToss;
	bool bRandomRotation;
	bool bBlockedFromAutoPickup;
	EFortPickupSourceTypeFlag SourceType;
	EFortPickupSpawnSource Source;
	class AFortPlayerController* OptionalOwnerPC;
	bool bPickupOnlyRelevantToOwner;

	Stack.StepCompiledIn(&WorldContextObject);
	Stack.StepCompiledIn(&Entry);
	Stack.StepCompiledIn(&PickupClass);
	Stack.StepCompiledIn(&Position);
	Stack.StepCompiledIn(&Direction);
	Stack.StepCompiledIn(&OverrideMaxStackCount);
	Stack.StepCompiledIn(&bToss);
	Stack.StepCompiledIn(&bRandomRotation);
	Stack.StepCompiledIn(&bBlockedFromAutoPickup);
	Stack.StepCompiledIn(&SourceType);
	Stack.StepCompiledIn(&Source);
	Stack.StepCompiledIn(&OptionalOwnerPC);
	Stack.StepCompiledIn(&bPickupOnlyRelevantToOwner);
	Stack.IncrementCode();

	return *Ret = SpawnPickup(Position, (UFortItemDefinition*)Entry.ItemDefinition, SourceType, Source, Entry.Count, 0, OptionalOwnerPC ? OptionalOwnerPC->MyFortPawn : nullptr);
}


AFortPickup* SpawnItemVariantPickupInWorldHook(UObject* Object, FFrame& Stack, AFortPickup** Ret)
{
	UObject* WorldContextObject;
	FSpawnItemVariantParams Params;
	Stack.StepCompiledIn(&WorldContextObject);
	Stack.StepCompiledIn(&Params);
	Stack.IncrementCode();
	auto Pickup = SpawnPickup(Params.Position, Params.WorldItemDefinition, Params.SourceType, Params.Source, Params.NumberToSpawn, 0);
	*Ret = Pickup;
	return Pickup;
}

bool SpawnLoot(ABuildingContainer* Object) {
	for (const auto& [OldTierGroup, RedirectedTierGroup] : ((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode)->RedirectAthenaLootTierGroups)
	{
		if (OldTierGroup == Object->SearchLootTierGroup)
		{
			Object->SearchLootTierGroup = RedirectedTierGroup;
			break;
		}
	}

	for (auto& Drop : PickLootDrops(Object->SearchLootTierGroup, Cast<AFortGameStateAthena>(UWorld::GetWorld()->GameState)->WorldLevel))
	{
		if (!Drop.ItemDefinition || Drop.Count <= 0)
			continue;
		FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);
		auto pickup = SpawnPickup(Loc, (UFortItemDefinition*)Drop.ItemDefinition, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Drop.Count, Drop.LoadedAmmo);
	}
	
	Object->bAlreadySearched = true;
	Object->OnRep_bAlreadySearched();
	Object->SearchBounceData.SearchAnimationCount++;
	Object->BounceContainer();
	return true;
}

#endif // !LOOTING


