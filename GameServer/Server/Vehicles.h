#ifndef VEHICLES
#define VEHICLES
#pragma once
#include "Utils.h"
void ServerMove(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState)
{
	UFortVehicleSkelMeshComponent* RootComp = (UFortVehicleSkelMeshComponent*)Pawn->RootComponent;
	
	auto Rotation = FQuatToRot(InState.Rotation);
	Rotation.pitch = 0.;
	Rotation.Roll = 0.;

	RootComp->K2_SetWorldLocationAndRotation(InState.Translation, Rotation, false, nullptr, true);
	RootComp->SetPhysicsLinearVelocity(InState.LinearVelocity, 0, FName(0));
	RootComp->SetPhysicsAngularVelocityInDegrees(InState.AngularVelocity, 0, FName(0));
}

AFortAthenaVehicle* SpawnSportsCar(FVector Loc, FRotator Rot) {
	static auto BGAClass = StaticFindObject<UClass>("/Script/Engine.BlueprintGeneratedClass");
	static auto VehicleClass = StaticLoadObject<UClass>("/Valet/SportsCar/Valet_SportsCar_Vehicle.Valet_SportsCar_Vehicle_C", BGAClass);
	auto actor = SpawnActor<AFortAthenaVehicle>(VehicleClass, Loc, Rot);
	VHook(actor, 0x112, ServerMove);
	VHook(actor, 0x111, ServerMove);
	return actor;
}

AFortAthenaVehicle* SpawnBasicSUV(FVector Loc, FRotator Rot) {
	static auto BGAClass = StaticFindObject<UClass>("/Script/Engine.BlueprintGeneratedClass");
	static auto VehicleClass = StaticLoadObject<UClass>("/BasicSUV/Vehicle/Valet_BasicSUV_Vehicle.Valet_BasicSUV_Vehicle_C", BGAClass);
	auto actor = SpawnActor<AFortAthenaVehicle>(VehicleClass, Loc, Rot);
	VHook(actor, 0x112, ServerMove);
	VHook(actor, 0x111, ServerMove);
	return actor;
}

AFortAthenaVehicle* SpawnDirtBike(FVector Loc, FRotator Rot) {
	static auto BGAClass = StaticFindObject<UClass>("/Script/Engine.BlueprintGeneratedClass");
	static auto VehicleClass = StaticLoadObject<UClass>("/Dirtbike/Vehicle/Motorcycle_DirtBike_Vehicle.Motorcycle_DirtBike_Vehicle_C", BGAClass);
	auto actor = SpawnActor<AFortAthenaVehicle>(VehicleClass, Loc, Rot);
	VHook(actor, 0x112, ServerMove);
	VHook(actor, 0x111, ServerMove);
	return actor;
}

AFortAthenaVehicle* SpawnLinedNotebook(FVector Loc, FRotator Rot) {
	static auto BGAClass = StaticFindObject<UClass>("/Script/Engine.BlueprintGeneratedClass");
	static auto VehicleClass = StaticLoadObject<UClass>("/LinedNotebookVehicle/Vehicle/BP_LinedNotebookVehicle_Hoverboard.BP_LinedNotebookVehicle_Hoverboard_C", BGAClass);
	auto actor = SpawnActor<AFortAthenaVehicle>(VehicleClass, Loc, Rot);
	VHook(actor, 0x112, ServerMove);
	VHook(actor, 0x111, ServerMove);
	return actor;
}

void SpawnCars() {

	static bool IsSpawned = false;
	if (!IsSpawned) {
		static auto BGAClass = StaticFindObject<UClass>("/Script/Engine.BlueprintGeneratedClass");
		static auto SportsCarClass = StaticLoadObject<UClass>("/Valet/LWSpawner/SportsCar/Valet_SportsCar_LWSpawner.Valet_SportsCar_LWSpawner_C", BGAClass);
		static auto BasicSUVClass = StaticLoadObject<UClass>("/BasicSUV/LWSpawner/LW_BasicSUV_Spawner.LW_BasicSUV_Spawner_C", BGAClass);
		static auto DirtBikeClass = StaticLoadObject<UClass>("/Dirtbike/LWSpawner/LW_DirtbikeVehicle_Spawner.LW_DirtbikeVehicle_Spawner_C", BGAClass);
		static auto LinedNotebookClass = StaticLoadObject<UClass>("/LinedNotebookVehicle/LWSpawner/LW_LinedNotebookVehicle_Spawner.LW_LinedNotebookVehicle_Spawner_C", BGAClass);
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortAthenaLivingWorldVehiclePointProvider::StaticClass(), &Actors);
		if (Actors.Num() > 150) {
			for (auto Actor : Actors) {
				auto Loc = Actor->K2_GetActorLocation() + FVector(0, 5, 0);
				auto Rot = Actor->K2_GetActorRotation();
				if (Actor->IsA(SportsCarClass))
					SpawnSportsCar(Loc, Rot);
				if (Actor->IsA(BasicSUVClass))
					SpawnBasicSUV(Loc, Rot);
				if (Actor->IsA(DirtBikeClass))
					SpawnDirtBike(Loc, Rot);
				if (Actor->IsA(LinedNotebookClass))
					SpawnLinedNotebook(Loc, Rot);
			}
			IsSpawned = true;
		}
		Actors.Free();
	}
}

#endif
