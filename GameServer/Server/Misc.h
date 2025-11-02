#ifndef MISC
#define MISC
#pragma once
#include "Utils.h"
#include "../SDK/SDK/FortniteGame_parameters.hpp"

bool NoMcp() {
    return false;
}

void KickPlayerHK() {
    return;
}

float GetMaxTickRate() {
    return 60.0f;
}

void ProcessEventHook(UObject* Object, UFunction* Function, void* Parameters) {
    if (!Object || !Function)
        return; 
    if (Object->IsA(AFortWeapon::StaticClass()) && Function->GetFullName().contains("GameplayCue_Weapons_Activation")) {
        auto Params = (Params::FortWeapon_GameplayCue_Weapons_Activation*)Parameters;
        std::cout << "EventType: " << (int)Params->EventType << "\n";
        std::cout << "Location: " << Params->Parameters.Location.X << " " << Params->Parameters.Location.Y << " "<<Params->Parameters.Location.Z << "\n";
    }
    return UObject::ProcessEventOG(Object, Function, Parameters);
}

inline __int64(__fastcall* MCP_DispatchRequestOG)(void* Profile, void* Context, int a3);
inline __int64 __fastcall MCP_DispatchRequest(void* Profile, void* Context, int a3)
{
    return MCP_DispatchRequestOG(Profile, Context, 3);
}

bool CanActivateAbility() {
    return true;
}

bool ReturnFalse() {
    return false;
}
bool ReturnTrue() {
    return true;
}

#endif // !MISC


