#ifndef MOVEMENTMODEEXTENSION
#define MOVEMENTMODEEXTENSION
#pragma once
#include "Utils.h"
// nothing to see rn thats just functions for checking
UWorld* (*ActorGetWorldOG)(AActor*);
UWorld* GetWorldHook(AActor* Actor) 
{
	auto offset = __int64(_ReturnAddress()) - InSDKUtils::GetImageBase();
	if (offset>= 0x1B009CC && offset<=0x1B00BAE)//proper as fuck
		return GetWorld();
	return ActorGetWorldOG(Actor);
}
char(__fastcall* sub_1B009CCOG)(int* a1, BYTE* a2, __int64 a3, BYTE* a4);
char __fastcall sub_1B009CC(int* a1, BYTE* a2, __int64 a3, BYTE* a4) {
	return sub_1B009CCOG(a1, a2, a3, a4);
}

void(__fastcall* sub_1B00E28OG)(DWORD* a1, FString a2, __int64 a3, __int64 a4);
void __fastcall sub_1B00E28(DWORD* a1, FString a2, __int64 a3, __int64 a4) {
	sub_1B00E28OG(a1, a2, a3, a4);
	//EFortCustomMovement
	//UFortMovementComp_CharacterAthena
	//AFortPlayerPawn
}
void(__fastcall* StartUpdatePhysOG)(UFortMovementComp_CharacterAthena* CharacterMovement, float a2, int a3);
void __fastcall StartUpdatePhys(UFortMovementComp_CharacterAthena* CharacterMovement, float a2, int a3) {

	//std::cout << "Starting Phys Update\n";
	return StartUpdatePhysOG(CharacterMovement, a2, a3);
}

#endif // !MOVEMENTMODEEXTENSION


