#pragma once
#include "Utils.h"
#include <thread>
#include <chrono>
#include <iostream>

// Asset-Namen der Medallions (ohne Pfad)
static const std::vector<std::string> MedallionAssetNames = {
    "PAID_TDM_BengalBasher_Regen",
    "PAID_TDM_CermonialGuard_Regen",
    "PAID_TDM_DiamondHeart_Regen",
    "PAID_TDM_MadameMoth_Regen",
    "PAID_TDM_ZebraScramle_Regen"
};

inline void StartMedallionRegen(AFortPlayerControllerAthena* PC)
{
    if (!PC || !PC->MyFortPawn) {
        std::cout << "[MedallionRegen] Kein Pawn oder PC!" << std::endl;
        return;
    }
    std::cout << "[MedallionRegen] Starte Schild-Regeneration für " << PC << std::endl;

    // Load sounds (move to local variables for lambda capture)
    USoundBase* LoopSound = StaticLoadObject<USoundBase>("/Game/Paprika/Sounds/Paprika_BossShieldRegen_Loop.Paprika_BossShieldRegen_Loop");
    USoundBase* EndSound = StaticLoadObject<USoundBase>("/Game/Paprika/Sounds/Paprika_BossSheildRegen_End.Paprika_BossSheildRegen_End");

    // Play loop sound at start
    if (LoopSound)
        PC->ClientPlaySound(LoopSound, 1.0f, 1.0f);

    std::thread([PC, EndSound]() {
        int shieldGiven = 0;
        while (shieldGiven < 50) {
            if (!PC->MyFortPawn) {
                std::cout << "[MedallionRegen] Pawn nicht mehr vorhanden!" << std::endl;
                break;
            }
            float currentShield = PC->MyFortPawn->GetShield();
            if (currentShield >= 50.0f) {
                std::cout << "[MedallionRegen] Schild bereits >= 50!" << std::endl;
                break;
            }
            float toGive = 5.0f;
            if (currentShield + toGive > 50.0f)
                toGive = 50.0f - currentShield;
            PC->MyFortPawn->SetShield(currentShield + toGive);
            std::cout << "[MedallionRegen] +" << toGive << " Schild (jetzt: " << (currentShield + toGive) << ")" << std::endl;
            shieldGiven += (int)toGive;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        // Play end sound after healing
        if (EndSound && PC && PC->MyFortPawn)
            PC->ClientPlaySound(EndSound, 1.0f, 1.0f);
        }).detach();
}

inline bool IsMedallionItem(UFortItemDefinition* Def) {
    if (!Def) return false;
    std::string path = Def->GetFullName();
    std::cout << "[MedallionRegen] Prüfe Item: " << path << std::endl;
    for (const auto& medName : MedallionAssetNames) {
        if (path.find(medName) != std::string::npos) {
            std::cout << "[MedallionRegen] Medallion erkannt: " << medName << std::endl;
            return true;
        }
    }
    return false;
}