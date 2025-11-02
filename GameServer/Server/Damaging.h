#ifndef DAMAGING
#define DAMAGING
#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "MedallionRegen.h"

inline void (*NetMulticastDamageCuesOG)(AFortPlayerPawnAthena* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData);
void NetMulticastDamageCues(AFortPlayerPawnAthena* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData)
{
	if (!Pawn || !Pawn->Controller)
		return NetMulticastDamageCuesOG(Pawn, SharedData, NonSharedData);

	;	if (Pawn && Pawn->CurrentWeapon)
	{
		AFortPlayerControllerAthena* PC = Cast<AFortPlayerControllerAthena>(Pawn->Controller);
		if (PC)
		{
			UpdateLoadedAmmo(PC, Pawn->CurrentWeapon);
		}
	}
	return NetMulticastDamageCuesOG(Pawn, SharedData, NonSharedData);
}

bool IsShotDirectionValid(APlayerPawn_Athena_C* Pawn, const FVector& ShotDirection, float MaxAngleDeviation = 5.0f)
{
	if (!Pawn || ShotDirection.IsZero())
		return false;

	FRotator PawnRotation = Pawn->K2_GetActorRotation();
	FVector PawnForwardVector = RotatorToVector(PawnRotation);

	FVector NormalizedShotDir = ShotDirection;
	NormalizedShotDir.Normalize();

	float DotProduct = UKismetMathLibrary::Dot_VectorVector(PawnForwardVector, NormalizedShotDir);
	float AngleRad = acosf(DotProduct);
	float AngleDeg = RadiansToDegrees(AngleRad);

	return AngleDeg <= MaxAngleDeviation;
}

double (*ProjectileRequest)(AFortLightweightProjectileManager* ProjectileManager, FWeakObjectPtr Pawn, FWeakObjectPtr a3, UClass** ProjectileConfig, FVector a5, FVector a6, ELightweightProjectileRequestType a7, int a8, unsigned int a9);
double ProjectileRequestHook(AFortLightweightProjectileManager* ProjectileManager, FWeakObjectPtr PawnObject, FWeakObjectPtr WeaponDef, UClass** ProjectileConfigPTR, FVector BuletStartPosition, FVector Direction, ELightweightProjectileRequestType Type, int a8, unsigned int a9) {
	auto res = ProjectileRequest(ProjectileManager, PawnObject, WeaponDef, ProjectileConfigPTR, BuletStartPosition, Direction, Type, a8, a9);
	auto Pawn = (APlayerPawn_Athena_C*)PawnObject.Get();
	if (!Pawn)
		return res;
	auto PC = (AFortPlayerControllerAthena*)Pawn->GetController();
	if (!PC)
		return res;

	auto WeaponData = Pawn->CurrentWeapon;
	if (!WeaponData)
		return res;

	auto WeaponRanged = Cast<AFortWeaponRanged>(WeaponData);
	if (!WeaponRanged)
		return res;

	auto ProjectileConfig = (AFortLightweightProjectileConfig*)(*ProjectileConfigPTR)->DefaultObject;
	if (!ProjectileConfig)
		return res;

	FFortItemEntry* SwapEntry = FindEntry(PC, WeaponData->WeaponData);
	if (!SwapEntry)
		return res;

	if (Pawn->K2_GetActorLocation().GetDistanceTo(BuletStartPosition) > 110)
		return res;

	if (!IsShotDirectionValid(Pawn, Direction, 100.0f))
		return res;

	auto ProjectileSpawn = FSpawnProjectileParams();
	ProjectileSpawn.SpawnLocation = BuletStartPosition;
	ProjectileSpawn.SpawnDirection = VectorToRotator(Direction);
	ProjectileSpawn.OptionalAssociatedItemDef = (UFortWorldItemDefinition*)SwapEntry->ItemDefinition;
	ProjectileSpawn.InitialSpeed = EvaluateScalableFloat(ProjectileConfig->Speed, 0);
	ProjectileSpawn.MaxSpeed = ProjectileSpawn.InitialSpeed;
	ProjectileSpawn.GravityScale = EvaluateScalableFloat(ProjectileConfig->GravityScale, 0);
	auto projectile = (AFortProjectileAthena*)UFortKismetLibrary::SpawnProjectileWithParams(AFortProjectileAthena::StaticClass(), WeaponRanged, ProjectileSpawn);
	UpdateLoadedAmmo(PC, WeaponRanged);
}

void(__fastcall* ServerNotifyPawnHitOG)(AFortProjectileAthena* projectile, FHitResult& Hit, const TArray<uint8>& Context);
void ServerNotifyPawnHitHK(AFortProjectileAthena* projectile, FHitResult& Hit, const TArray<uint8>& Context) {
	ServerNotifyPawnHitOG(projectile, Hit, Context);
	auto Owner = projectile->GetOwnerPlayerController();

	auto OwnerWeapon = (AFortWeaponRanged*)projectile->GetOwnerWeapon();
	auto Component = Hit.Component.Get();
	if (Component) {
		auto Actor = Component->GetOwner();
		if (Actor) {
			if (Owner) {
				auto Pawn = Owner->MyFortPawn;
				if (Pawn && Pawn->K2_GetActorLocation().GetDistanceTo(Actor->K2_GetActorLocation()) > 10000)
					return;
			}
			auto WeaponStats = GetStats(OwnerWeapon->GetWeaponData());
			if (WeaponStats) {
				if (Actor->bCanBeDamaged == 1) {
					if (auto BuildingActor = Cast<ABuildingSMActor>(Actor)) {
						float Damage = WeaponStats->EnvDmgPB;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;
						FAthenaBatchedDamageGameplayCues_NonShared NonSharedCue{};
						NonSharedCue.HitActor = Actor;
						NonSharedCue.NonPlayerHitActor = Actor;

						FGameplayTagContainer DamageTags;

						FVector Momentum = projectile->GetVelocity();

						FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();

						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);
						float Healthleft = BuildingActor->GetHealth() - Damage;
						BuildingActor->SetHealth(Healthleft);
						BuildingActor->ForceNetUpdate();
						if (BuildingActor->GetHealth() <= 0)
							BuildingActor->K2_DestroyActor();

					}
					if (auto Vehicle = Cast<AFortAthenaVehicle>(Actor)) {
						float Damage = WeaponStats->EnvDmgPB;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;
						FAthenaBatchedDamageGameplayCues_NonShared	NonSharedCue{};
						NonSharedCue.HitActor = Actor;

						FGameplayTagContainer DamageTags;

						FVector Momentum = projectile->GetVelocity();

						FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();
						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);

						Vehicle->HealthSet->Health.CurrentValue -= Damage;
						Vehicle->OnRep_HealthSet();
						if (Vehicle->HealthSet->Health.CurrentValue <= 0)
						{
							Vehicle->DestroyVehicle();
						}
					}

					if (auto SuperDingo = Cast<AAthenaSuperDingo>(Actor)) {
						float Damage = WeaponStats->EnvDmgPB;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;
						FAthenaBatchedDamageGameplayCues_NonShared NonSharedCue{};
						NonSharedCue.HitActor = Actor;

						FGameplayTagContainer DamageTags;

						FVector Momentum = projectile->GetVelocity();

						FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();
						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);

						float Healthleft = SuperDingo->GetHealth() - Damage;
						SuperDingo->SetHealth(Healthleft);
						if (SuperDingo->GetHealth() <= 0)
							SuperDingo->K2_DestroyActor();
					}

					if (auto CrashPad = Cast<ABuildingGameplayActorCrashpad>(Actor)) {
						float Damage = WeaponStats->EnvDmgPB;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;
						FAthenaBatchedDamageGameplayCues_NonShared NonSharedCue{};
						NonSharedCue.HitActor = Actor;

						FGameplayTagContainer DamageTags;

						FVector Momentum = projectile->GetVelocity();

						FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();
						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);

						float Healthleft = CrashPad->GetHealth() - Damage;
						CrashPad->SetHealth(Healthleft);
						if (CrashPad->GetHealth() <= 0)
							CrashPad->K2_DestroyActor();
					}

					if (auto Baloon = Cast<ABuildingGameplayActorBalloon>(Actor)) {
						float Damage = WeaponStats->EnvDmgPB;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;
						FAthenaBatchedDamageGameplayCues_NonShared NonSharedCue{};
						NonSharedCue.HitActor = Actor;

						FGameplayTagContainer DamageTags;

						FVector Momentum = projectile->GetVelocity();

						FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();
						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);

						float Healthleft = Baloon->GetHealth() - Damage;
						Baloon->SetHealth(Healthleft);
						if (Baloon->GetHealth() <= 0)
							Baloon->K2_DestroyActor();
					}

					if (auto TargetPawn = Cast<AFortPlayerPawn>(Actor)) {
						if (auto TargetController = Cast<AFortPlayerController>(TargetPawn->GetController())) {

						}
						float Multiplier = 1;
						if (Hit.BoneName.ToString() == "head")
							Multiplier = WeaponStats->DamageZone_Critical;
						float Damage = WeaponStats->DmgPB * Multiplier;
						FAthenaBatchedDamageGameplayCues_Shared SharedCue{};
						SharedCue.Location = (FVector_NetQuantize10)Hit.ImpactPoint;
						SharedCue.Normal = Hit.ImpactNormal;
						SharedCue.Magnitude = Damage;
						SharedCue.bWeaponActivate = true;
						SharedCue.bIsFatal = false;
						SharedCue.bIsCritical = Hit.BoneName.ToString() == "head";
						SharedCue.bIsShield = false;
						SharedCue.bIsShieldDestroyed = false;
						SharedCue.bIsShieldApplied = false;
						SharedCue.bIsBallistic = true;
						SharedCue.bIsBeam = false;
						SharedCue.bIsValid = true;

						FAthenaBatchedDamageGameplayCues_NonShared NonSharedCue{};
						NonSharedCue.HitActor = Actor;
						NonSharedCue.NonPlayerHitActor = nullptr;


						if (GetGamePhaseLogic()->GamePhase != EAthenaGamePhase::Warmup) {
							auto PlayerShield = TargetPawn->GetShield();
							auto PlayerHealth = TargetPawn->GetHealth();
							if (PlayerShield > 0) {
								SharedCue.bIsShield = true;
								if (PlayerShield < Damage)
								{
									SharedCue.bIsShieldDestroyed = true;
									TargetPawn->SetShield(0);
									TargetPawn->SetHealth(PlayerHealth - (Damage - PlayerShield));
									TargetPawn->ForceNetUpdate();
								}
								else
								{
									TargetPawn->SetShield(PlayerShield - Damage);
									TargetPawn->ForceNetUpdate();
								}
							}
							else if (PlayerHealth > Damage) {
								TargetPawn->SetHealth(PlayerHealth - Damage);
								TargetPawn->ForceNetUpdate();
							}
							else {
								TargetPawn->ForceKill(FGameplayTag(), Owner, OwnerWeapon);
							}

						}
						Owner->MyFortPawn->NetMulticast_Athena_BatchedDamageCues(SharedCue, NonSharedCue);

					}
				}
			}
		}
	}
}

#endif // !