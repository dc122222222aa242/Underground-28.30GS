#ifndef GAMEPHASES
#define GAMEPHASES
#include "Utils.h"
#include "SafeZoneLocations.h"
#include <chrono>
#include <thread>

bool AllowAircraftJumping = false;
void(__fastcall* SetGamePhase)(UFortGameStateComponent_BattleRoyaleGamePhaseLogic* GamePhaseLogic, EAthenaGamePhase GamePhase) = decltype(SetGamePhase)(InSDKUtils::GetImageBase() + 0x9B6DB18);

float LastPhaseIncrementTime = 0.0f;
const float PhaseIncrementInterval = 30.0f;

bool isRunning = false;

void timer(int sec)
{
	Sleep(sec * 1000);
}

void InitSafeZones(AFortGameModeAthena* GameMode) {
	auto GameState = (AFortGameStateBR*)GameMode->GameState;
	auto GamePhaseLogic = GetGamePhaseLogic();
	if (!GamePhaseLogic)
		return;
	unsigned int RandomSeed = static_cast<unsigned int>(
		std::chrono::system_clock::now().time_since_epoch().count()
		);
	GenerateStormCircles(GameState->MapInfo, RandomSeed);
	auto SafeZoneIndicator = SpawnActor<AFortSafeZoneIndicator>(GamePhaseLogic->SafeZoneIndicatorClass.Get(), {});
	FFortSafeZoneDefinition SafeZoneDefinition = GameState->MapInfo->SafeZoneDefinition;
	float SafeZoneCount = EvaluateScalableFloat(SafeZoneDefinition.Count, 0);

	if (SafeZoneIndicator->SafeZonePhases.IsValid())
		SafeZoneIndicator->SafeZonePhases.Free();

	const float TimeSeconds = (float)UGameplayStatics::GetTimeSeconds(GameState);
	const float ServerWorldTimeSeconds = GameState->ServerWorldTimeSecondsDelta + TimeSeconds;

	for (float i = 0; i < SafeZoneCount; i++)
	{
		FFortSafeZonePhaseInfo SafeZonePhaseInfo{};
		SafeZonePhaseInfo.Radius = EvaluateScalableFloat(SafeZoneDefinition.Radius, i);
		SafeZonePhaseInfo.WaitTime = EvaluateScalableFloat(SafeZoneDefinition.WaitTime, i);
		SafeZonePhaseInfo.ShrinkTime = EvaluateScalableFloat(SafeZoneDefinition.ShrinkTime, i);
		SafeZonePhaseInfo.PlayerCap = (int)EvaluateScalableFloat(SafeZoneDefinition.PlayerCapSolo, i);

		SafeZonePhaseInfo.DamageInfo = FFortSafeZoneDamageInfo();
		UDataTableFunctionLibrary::EvaluateCurveTableRow(GameState->AthenaGameDataTable, Conv_StringToName(L"Default.SafeZone.Damage"), i, nullptr, &SafeZonePhaseInfo.DamageInfo.Damage, FString());
		if (i == 0.f)
			SafeZonePhaseInfo.DamageInfo.Damage = 0.01f;

		SafeZonePhaseInfo.DamageInfo.bPercentageBasedDamage = true;
		SafeZonePhaseInfo.TimeBetweenStormCapDamage = EvaluateScalableFloat(GamePhaseLogic->TimeBetweenStormCapDamage, i);
		SafeZonePhaseInfo.StormCapDamagePerTick = EvaluateScalableFloat(GamePhaseLogic->StormCapDamagePerTick, i);
		SafeZonePhaseInfo.StormCampingIncrementTimeAfterDelay = EvaluateScalableFloat(GamePhaseLogic->StormCampingIncrementTimeAfterDelay, i);
		SafeZonePhaseInfo.StormCampingInitialDelayTime = EvaluateScalableFloat(GamePhaseLogic->StormCampingInitialDelayTime, i);
		SafeZonePhaseInfo.MegaStormGridCellThickness = (int)EvaluateScalableFloat(SafeZoneDefinition.MegaStormGridCellThickness, i);
		auto Center = StormCircles[i].Center;
		SafeZonePhaseInfo.Center = Center;

		SafeZoneIndicator->SafeZonePhases.Add(SafeZonePhaseInfo);

		SafeZoneIndicator->PhaseCount++;
	}
	SafeZoneIndicator->OnRep_PhaseCount();

	SafeZoneIndicator->PreviousCenter = FVector_NetQuantize100(SafeZoneIndicator->SafeZonePhases[0].Center);
	SafeZoneIndicator->PreviousRadius = 1000000;

	SafeZoneIndicator->NextCenter = FVector_NetQuantize100(SafeZoneIndicator->SafeZonePhases[0].Center);
	SafeZoneIndicator->NextRadius = SafeZoneIndicator->SafeZonePhases[0].Radius;

	SafeZoneIndicator->SafeZoneStartShrinkTime = ServerWorldTimeSeconds + SafeZoneIndicator->SafeZonePhases[0].WaitTime;
	SafeZoneIndicator->SafeZoneFinishShrinkTime = SafeZoneIndicator->SafeZoneStartShrinkTime + SafeZoneIndicator->SafeZonePhases[0].ShrinkTime;
	SafeZoneIndicator->CurrentDamageInfo = SafeZoneIndicator->SafeZonePhases[0].DamageInfo;

	SafeZoneIndicator->CurrentPhase = 0;
	SafeZoneIndicator->OnRep_CurrentPhase();

	SafeZoneIndicator->OnSafeZoneStateChange(EFortSafeZoneState::Starting, true);
	GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
	GamePhaseLogic->SafeZoneIndicator = SafeZoneIndicator;
	GamePhaseLogic->OnRep_SafeZoneIndicator();

	const float TimeNow = (float)UGameplayStatics::GetTimeSeconds(GameState);
	LastPhaseIncrementTime = GameState->ServerWorldTimeSecondsDelta + TimeNow;
}

void StartSafeZonePhase(AFortGameModeAthena* GameMode, int32 PhaseIndex) {
	auto GameState = (AFortGameStateBR*)GameMode->GameState;
	auto GamePhaseLogic = GetGamePhaseLogic();

	if (!GamePhaseLogic)
		return;

	const float TimeSeconds = (float)UGameplayStatics::GetTimeSeconds(GameState);
	const float ServerWorldTimeSeconds = GameState->ServerWorldTimeSecondsDelta + TimeSeconds;

	auto SafeZoneIndicator = GamePhaseLogic->SafeZoneIndicator;

	if (!SafeZoneIndicator)
		return;

	if (!SafeZoneIndicator->SafeZonePhases.IsValidIndex(PhaseIndex))
		return;

	int PrevPhaseIndex = SafeZoneIndicator->CurrentPhase;
	if (PrevPhaseIndex < 0 || !SafeZoneIndicator->SafeZonePhases.IsValidIndex(PrevPhaseIndex))
		PrevPhaseIndex = PhaseIndex - 1;
	if (PrevPhaseIndex < 0) PrevPhaseIndex = 0;

	FFortSafeZonePhaseInfo PrevInfo = SafeZoneIndicator->SafeZonePhases[PrevPhaseIndex];
	FFortSafeZonePhaseInfo NextInfo = SafeZoneIndicator->SafeZonePhases[PhaseIndex];

	SafeZoneIndicator->PreviousCenter = FVector_NetQuantize100(PrevInfo.Center);
	SafeZoneIndicator->PreviousRadius = PrevInfo.Radius;

	SafeZoneIndicator->NextCenter = FVector_NetQuantize100(NextInfo.Center);
	SafeZoneIndicator->NextRadius = NextInfo.Radius;
	SafeZoneIndicator->NextMegaStormGridCellThickness = NextInfo.MegaStormGridCellThickness;

	SafeZoneIndicator->CurrentDamageInfo = PrevInfo.DamageInfo;
	SafeZoneIndicator->OnRep_CurrentDamageInfo();

	SafeZoneIndicator->SafeZoneStartShrinkTime = ServerWorldTimeSeconds + NextInfo.WaitTime;
	SafeZoneIndicator->SafeZoneFinishShrinkTime = SafeZoneIndicator->SafeZoneStartShrinkTime + NextInfo.ShrinkTime;

	GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;

	LastPhaseIncrementTime = ServerWorldTimeSeconds;
}


void StartAircraftPhase(AFortGameModeAthena* GameMode) {
	static void(__fastcall * ClientEnterAircraft)(UFortControllerComponent_Aircraft*, AFortAthenaAircraft*) = decltype(ClientEnterAircraft)(InSDKUtils::GetImageBase() + 0x89D5D98);
	auto GameState = (AFortGameStateBR*)GameMode->GameState;
	auto GamePhaseLogic = GetGamePhaseLogic();
	if (bLateGame && !staticZones) {
		unsigned int RandomSeed = static_cast<unsigned int>(
			std::chrono::system_clock::now().time_since_epoch().count()
			);

		isRunning = true;
		GenerateStormCircles(GameState->MapInfo, RandomSeed);
		auto SafeZoneIndicator = SpawnActor<AFortSafeZoneIndicator>(GamePhaseLogic->SafeZoneIndicatorClass.Get(), {});
		FFortSafeZoneDefinition SafeZoneDefinition = GameState->MapInfo->SafeZoneDefinition;
		float SafeZoneCount = EvaluateScalableFloat(SafeZoneDefinition.Count, 0);

		if (SafeZoneIndicator->SafeZonePhases.IsValid())
			SafeZoneIndicator->SafeZonePhases.Free();

		const float TimeSeconds = (float)UGameplayStatics::GetTimeSeconds(GameState);
		const float ServerWorldTimeSeconds = GameState->ServerWorldTimeSecondsDelta + TimeSeconds;

		for (float i = 0; i < SafeZoneCount; i++)
		{
			FFortSafeZonePhaseInfo SafeZonePhaseInfo{};
			SafeZonePhaseInfo.Radius = EvaluateScalableFloat(SafeZoneDefinition.Radius, i);
			SafeZonePhaseInfo.WaitTime = EvaluateScalableFloat(SafeZoneDefinition.WaitTime, i);
			SafeZonePhaseInfo.ShrinkTime = EvaluateScalableFloat(SafeZoneDefinition.ShrinkTime, i);
			SafeZonePhaseInfo.PlayerCap = (int)EvaluateScalableFloat(SafeZoneDefinition.PlayerCapSolo, i);

			SafeZonePhaseInfo.DamageInfo = FFortSafeZoneDamageInfo();
			UDataTableFunctionLibrary::EvaluateCurveTableRow(GameState->AthenaGameDataTable, Conv_StringToName(L"Default.SafeZone.Damage"), i, nullptr, &SafeZonePhaseInfo.DamageInfo.Damage, FString());
			if (i == 0.f)
				SafeZonePhaseInfo.DamageInfo.Damage = 0.01f;

			SafeZonePhaseInfo.DamageInfo.bPercentageBasedDamage = true;
			SafeZonePhaseInfo.TimeBetweenStormCapDamage = EvaluateScalableFloat(GamePhaseLogic->TimeBetweenStormCapDamage, i);
			SafeZonePhaseInfo.StormCapDamagePerTick = EvaluateScalableFloat(GamePhaseLogic->StormCapDamagePerTick, i);
			SafeZonePhaseInfo.StormCampingIncrementTimeAfterDelay = EvaluateScalableFloat(GamePhaseLogic->StormCampingIncrementTimeAfterDelay, i);
			SafeZonePhaseInfo.StormCampingInitialDelayTime = EvaluateScalableFloat(GamePhaseLogic->StormCampingInitialDelayTime, i);
			SafeZonePhaseInfo.MegaStormGridCellThickness = (int)EvaluateScalableFloat(SafeZoneDefinition.MegaStormGridCellThickness, i);
			auto Center = StormCircles[i].Center;
			SafeZonePhaseInfo.Center = Center;

			SafeZoneIndicator->SafeZonePhases.Add(SafeZonePhaseInfo);

			SafeZoneIndicator->PhaseCount++;
		}
		SafeZoneIndicator->OnRep_PhaseCount();

		FFortSafeZonePhaseInfo& PhaseInfo = SafeZoneIndicator->SafeZonePhases[6];

		SafeZoneIndicator->PreviousCenter = FVector_NetQuantize100(PhaseInfo.Center);
		SafeZoneIndicator->PreviousRadius = PhaseInfo.Radius;

		SafeZoneIndicator->NextCenter = FVector_NetQuantize100(PhaseInfo.Center);
		SafeZoneIndicator->NextRadius = PhaseInfo.Radius;
		SafeZoneIndicator->NextMegaStormGridCellThickness = PhaseInfo.MegaStormGridCellThickness;

		SafeZoneIndicator->SafeZoneStartShrinkTime = ServerWorldTimeSeconds + 30.0f;
		SafeZoneIndicator->SafeZoneFinishShrinkTime = SafeZoneIndicator->SafeZoneStartShrinkTime + 5.0f;

		SafeZoneIndicator->CurrentDamageInfo = PhaseInfo.DamageInfo;
		SafeZoneIndicator->OnRep_CurrentDamageInfo();

		SafeZoneIndicator->CurrentPhase = 5;
		SafeZoneIndicator->OnRep_CurrentPhase();

		GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
		GamePhaseLogic->SafeZoneIndicator = SafeZoneIndicator;
		GamePhaseLogic->OnRep_SafeZoneIndicator();

		auto OriginalInfo = GameState->FlightPathMidLine;
		FAircraftFlightInfo NewInfo;

		FVector ZoneCenter = SafeZoneIndicator->SafeZonePhases[6].Center;
		float Radius = SafeZoneIndicator->NextRadius;

		NewInfo.FlightStartLocation = (FVector_NetQuantize100)ZoneCenter;
		NewInfo.FlightStartLocation.Z = 15000;

		FVector FlightDirection = FVector(1, 1, 0);
		FlightDirection = GetSafeNormal(FlightDirection);

		NewInfo.FlightStartRotation = VectorToRotator(FlightDirection);
		NewInfo.FlightSpeed = OriginalInfo.FlightSpeed / 3;

		float FlightDistance = Radius * 2;
		float FlightTime = FlightDistance / NewInfo.FlightSpeed;

		NewInfo.TimeTillDropStart = 0.0f;
		NewInfo.TimeTillDropEnd = FlightTime;
		NewInfo.TimeTillFlightEnd = FlightTime + 10;

		if (NewInfo.TimeTillDropEnd < 0.0f) NewInfo.TimeTillDropEnd = 0.0f;
		if (NewInfo.TimeTillFlightEnd < 0.0f) NewInfo.TimeTillFlightEnd = 0.0f;

		GameState->FlightPathMidLine = NewInfo;
		SetGamePhase(GamePhaseLogic, EAthenaGamePhase::SafeZones);
	}
	else
	{
		if (staticZones)
		{
			unsigned int RandomSeed = static_cast<unsigned int>(
				std::chrono::system_clock::now().time_since_epoch().count()
				);
			GenerateStormCircles(GameState->MapInfo, RandomSeed);

			auto SafeZoneIndicator = SpawnActor<AFortSafeZoneIndicator>(
				GamePhaseLogic->SafeZoneIndicatorClass.Get(), {}
			);

			FFortSafeZoneDefinition SafeZoneDefinition = GameState->MapInfo->SafeZoneDefinition;
			float SafeZoneCount = EvaluateScalableFloat(SafeZoneDefinition.Count, 0);

			if (SafeZoneIndicator->SafeZonePhases.IsValid())
				SafeZoneIndicator->SafeZonePhases.Free();

			const float TimeSeconds = (float)UGameplayStatics::GetTimeSeconds(GameState);
			const float ServerWorldTimeSeconds = GameState->ServerWorldTimeSecondsDelta + TimeSeconds;


			FFortSafeZonePhaseInfo StaticPhase{};
			StaticPhase.Radius = 10000.0f;  
			StaticPhase.WaitTime = 999999.0f; 
			StaticPhase.ShrinkTime = 0.0f;  
			StaticPhase.PlayerCap = 100;
			StaticPhase.DamageInfo = FFortSafeZoneDamageInfo();
			StaticPhase.DamageInfo.Damage = 10.0f; 
			StaticPhase.DamageInfo.bPercentageBasedDamage = false;
			StaticPhase.TimeBetweenStormCapDamage = 1.0f;
			StaticPhase.StormCapDamagePerTick = 10.0f;
			StaticPhase.StormCampingIncrementTimeAfterDelay = 999999.0f;
			StaticPhase.StormCampingInitialDelayTime = 999999.0f;
			StaticPhase.MegaStormGridCellThickness = 0;


			auto Center = StormCircles[0].Center;
			StaticPhase.Center = Center;

			SafeZoneIndicator->SafeZonePhases.Add(StaticPhase);
			SafeZoneIndicator->PhaseCount = 1;
			SafeZoneIndicator->OnRep_PhaseCount();

			SafeZoneIndicator->PreviousCenter = FVector_NetQuantize100(Center);
			SafeZoneIndicator->PreviousRadius = StaticPhase.Radius;
			SafeZoneIndicator->NextCenter = FVector_NetQuantize100(Center);
			SafeZoneIndicator->NextRadius = StaticPhase.Radius;
			SafeZoneIndicator->NextMegaStormGridCellThickness = 0;

			SafeZoneIndicator->SafeZoneStartShrinkTime = ServerWorldTimeSeconds + 9999999.0f;
			SafeZoneIndicator->SafeZoneFinishShrinkTime = SafeZoneIndicator->SafeZoneStartShrinkTime + 9999999.0f;

			SafeZoneIndicator->CurrentDamageInfo = StaticPhase.DamageInfo;
			SafeZoneIndicator->OnRep_CurrentDamageInfo();

			SafeZoneIndicator->CurrentPhase = 0;
			SafeZoneIndicator->OnRep_CurrentPhase();

			GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
			GamePhaseLogic->SafeZoneIndicator = SafeZoneIndicator;
			GamePhaseLogic->OnRep_SafeZoneIndicator();

			auto OriginalInfo = GameState->FlightPathMidLine;
			FAircraftFlightInfo NewInfo;

			FVector ZoneCenter = Center;
			float Radius = StaticPhase.Radius;

			NewInfo.FlightStartLocation = (FVector_NetQuantize100)ZoneCenter;
			NewInfo.FlightStartLocation.Z = 15000; 
			FVector FlightDirection = FVector(1, 1, 0);
			FlightDirection = GetSafeNormal(FlightDirection);
			NewInfo.FlightStartRotation = VectorToRotator(FlightDirection);
			NewInfo.FlightSpeed = 0; 
			NewInfo.TimeTillDropStart = 0.0f;
			NewInfo.TimeTillDropEnd = 0.1f;
			NewInfo.TimeTillFlightEnd = 0.1f;

			GameState->FlightPathMidLine = NewInfo;
			SetGamePhase(GamePhaseLogic, EAthenaGamePhase::SafeZones);

			auto aircraftclass = StaticLoadObject<UClass>("/Game/Athena/Aircraft/AthenaAircraft.AthenaAircraft_C");
			auto aircraft = AFortAthenaAircraft::SpawnAircraft(GetWorld(), aircraftclass, GameState->FlightPathMidLine);

			GamePhaseLogic->AircraftRealStartTime = GetStatics()->GetTimeSeconds(GetWorld());
			GamePhaseLogic->AircraftStartTime = GetStatics()->GetTimeSeconds(GetWorld());
			aircraft->bWasSpawnedForAircraftPhase = true;

			TArray<AFortAthenaAircraft*> Aircrafts;
			Aircrafts.Add(aircraft);
			GamePhaseLogic->SetAircrafts(Aircrafts);

			GamePhaseLogic->WarmupCountdownEndTime = GetStatics()->GetTimeSeconds(GetWorld());
			GamePhaseLogic->OnRep_WarmupCountdownEndTime();

			for (auto Player : GameMode->AlivePlayers)
			{
				auto PC = (AFortPlayerControllerAthena*)Player;
				if (PC && PC->Pawn)
					PC->Pawn->K2_DestroyActor();
			}

			aircraft->K2_SetActorLocation(NewInfo.FlightStartLocation, false, nullptr, false);

			std::thread([GameMode, aircraft]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(3000));
				UAthenaAIServicePlayerBots* AIServicePlayerBots = UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld());
				AIServicePlayerBots->CachedGamePhaseLogic = GetGamePhaseLogic();

				for (auto* BotController : GameMode->AliveBots)
				{
					auto DropLoc = aircraft->K2_GetActorLocation();
					DropLoc.Z -= 10000.f;
					BotController->ThankBusDriver();
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
					BotController->PlayerBotPawn->BeginSkydiving(true);
					BotController->PlayerBotPawn->bIsSkydiving = true;
					BotController->PlayerBotPawn->OnRep_IsSkydiving(false);
					BotController->PlayerBotPawn->K2_TeleportTo(DropLoc, FRotator());
				}
				}).detach();
		}

	}

	auto aircraftclass = StaticLoadObject<UClass>("/Game/Athena/Aircraft/AthenaAircraft.AthenaAircraft_C");
	auto aircraft = AFortAthenaAircraft::SpawnAircraft(GetWorld(), aircraftclass, GameState->FlightPathMidLine);
	GamePhaseLogic->AircraftRealStartTime = GetStatics()->GetTimeSeconds(GetWorld());
	GamePhaseLogic->AircraftStartTime = GetStatics()->GetTimeSeconds(GetWorld());
	aircraft->bWasSpawnedForAircraftPhase = true;

	TArray<AFortAthenaAircraft*> Aircrafts = TArray<AFortAthenaAircraft*>();
	Aircrafts.Add(aircraft);
	GamePhaseLogic->SetAircrafts(Aircrafts);

	GamePhaseLogic->WarmupCountdownEndTime = GetStatics()->GetTimeSeconds(GetWorld());
	GamePhaseLogic->OnRep_WarmupCountdownEndTime();

	for (auto Player : GameMode->AlivePlayers) {
		auto PC = (AFortPlayerControllerAthena*)Player;
		PC->Pawn->K2_DestroyActor();
	}


	SetGamePhase(GamePhaseLogic, EAthenaGamePhase::Aircraft);
	aircraft->StartFlightPath(GetStatics()->GetTimeSeconds(GetWorld()));

	std::thread([GameMode, aircraft]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		UAthenaAIServicePlayerBots* AIServicePlayerBots = UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld());
		AIServicePlayerBots->CachedGamePhaseLogic = GetGamePhaseLogic();
		for (auto* BotController : GameMode->AliveBots) {
			auto Wow = aircraft->K2_GetActorLocation();
			Wow.Z -= 10000.f;
			BotController->ThankBusDriver();
			std::this_thread::sleep_for(std::chrono::milliseconds(400));
			BotController->PlayerBotPawn->BeginSkydiving(true);
			BotController->PlayerBotPawn->bIsSkydiving = true;
			BotController->PlayerBotPawn->OnRep_IsSkydiving(false);
			BotController->PlayerBotPawn->K2_TeleportTo(Wow, FRotator());
		}
		}).detach();
}

void StartSafeZones(AFortGameModeAthena* GameMode) {

	auto GameState = (AFortGameStateBR*)GameMode->GameState;
	auto GamePhaseLogic = GetGamePhaseLogic();

	InitSafeZones(GameMode);

	SetGamePhase(GamePhaseLogic, EAthenaGamePhase::SafeZones);

}

void GamePhasesTick() {

	static float LastBuildWipeTime = 0.0f;

	auto GameMode = GetGameMode();

	if (!GameMode)
		return;

	auto GameState = (AFortGameStateBR*)GameMode->GameState;

	if (!GameState)
		return;

	auto GamePhaseLogic = GetGamePhaseLogic();

	if (!GamePhaseLogic)
		return;

	auto CurrentTime = GetStatics()->GetTimeSeconds(GetWorld());

	if (staticZones && GamePhaseLogic->GamePhase == EAthenaGamePhase::SafeZones) {
		if (CurrentTime - LastBuildWipeTime >= 420.0f) {
			for (int i = 0; i < Builds.Num(); ++i) {
				if (Builds[i]) {
					Builds[i]->K2_DestroyActor();
				}
			}
			Builds = {};
			LastBuildWipeTime = CurrentTime;
		}
	}

	if (GamePhaseLogic->GamePhase == EAthenaGamePhase::Warmup) {

		if (GamePhaseLogic->WarmupCountdownEndTime < CurrentTime) {

			StartAircraftPhase(GameMode);
		}
	}

	if (GamePhaseLogic->GamePhase == EAthenaGamePhase::Aircraft) {
		auto Aircraft = GamePhaseLogic->Aircrafts_GameState[0].Get();

		if (Aircraft) {
			if (Aircraft->DropEndTime < CurrentTime) {
				for (auto Player : GameMode->AlivePlayers)
				{
					if (Player->IsInAircraft())
					{
						Player->GetAircraftComponent()->ServerAttemptAircraftJump(FRotator());
					}
				}
				Aircraft->K2_DestroyActor();
				if (!bLateGame)
					StartSafeZones(GameMode);
				else {
					SetGamePhase(GamePhaseLogic, EAthenaGamePhase::SafeZones);
				}
				return;
			}
			if (Aircraft->DropStartTime < CurrentTime)
				AllowAircraftJumping = true;
		}

	}

	if (GamePhaseLogic->GamePhase == EAthenaGamePhase::SafeZones) {
		auto SafeZoneIndicator = GamePhaseLogic->SafeZoneIndicator;

		if (SafeZoneIndicator) {
			const float TimeNow = GetStatics()->GetTimeSeconds(GetWorld());

			if (TimeNow >= SafeZoneIndicator->SafeZoneStartShrinkTime &&
				GamePhaseLogic->GamePhaseStep != EAthenaGamePhaseStep::StormShrinking)
			{
				GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormShrinking;
			}

			if (TimeNow >= SafeZoneIndicator->SafeZoneFinishShrinkTime &&
				GamePhaseLogic->GamePhaseStep == EAthenaGamePhaseStep::StormShrinking)
			{
				GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;

				int CompletedPhase = SafeZoneIndicator->CurrentPhase + 1;

				if (SafeZoneIndicator->SafeZonePhases.IsValidIndex(CompletedPhase)) {
					SafeZoneIndicator->CurrentPhase = CompletedPhase;
					SafeZoneIndicator->OnRep_CurrentPhase();

					SafeZoneIndicator->CurrentDamageInfo =
						SafeZoneIndicator->SafeZonePhases[CompletedPhase].DamageInfo;
					SafeZoneIndicator->OnRep_CurrentDamageInfo();

					int NextPhaseToQueue = CompletedPhase + 1;
					if (SafeZoneIndicator->SafeZonePhases.IsValidIndex(NextPhaseToQueue)) {
						StartSafeZonePhase(GameMode, NextPhaseToQueue);
					}
					else {
						SafeZoneIndicator->SafeZoneStartShrinkTime = TimeNow;
						SafeZoneIndicator->SafeZoneFinishShrinkTime = TimeNow + 10.0f; 
						GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
					}
				}
				else {
					int LastPhaseIndex = SafeZoneIndicator->SafeZonePhases.Num() - 1;
					if (LastPhaseIndex >= 0) {
						SafeZoneIndicator->CurrentPhase = LastPhaseIndex;
						SafeZoneIndicator->OnRep_CurrentPhase();

						SafeZoneIndicator->CurrentDamageInfo =
							SafeZoneIndicator->SafeZonePhases[LastPhaseIndex].DamageInfo;
						SafeZoneIndicator->OnRep_CurrentDamageInfo();

						SafeZoneIndicator->SafeZoneStartShrinkTime = TimeNow;
						SafeZoneIndicator->SafeZoneFinishShrinkTime = TimeNow + 10.0f; 
						GamePhaseLogic->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
					}
				}
			}


		}

		for (auto Player : GameMode->AlivePlayers) {
			if (Player->MyFortPawn) {
				bool bIsInSafeZone = GamePhaseLogic->IsInCurrentSafeZone(Player->MyFortPawn->K2_GetActorLocation(), true);
				auto Pawn = Player->MyFortPawn;
				if (Pawn->bIsInsideSafeZone != bIsInSafeZone && Pawn->bIsInAnyStorm != !bIsInSafeZone)
				{
					Pawn->bIsInAnyStorm = !bIsInSafeZone;
					Pawn->OnRep_IsInAnyStorm();
					Pawn->bIsInsideSafeZone = bIsInSafeZone;
					Pawn->OnRep_IsInsideSafeZone();
				}
			}
		}
	}

}

#endif
