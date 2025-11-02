#pragma once
#include <iostream>
#include <vector>
#include <string>

namespace Lootpool
{
    namespace Weapons
    {
        inline std::vector<std::string> First = {
            // striker ar
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_DPS_Athena_UR_Boss.WID_Assault_Paprika_DPS_Athena_UR_Boss",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_DPS_Athena_R.WID_Assault_Paprika_DPS_Athena_R",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_DPS_Athena_VR.WID_Assault_Paprika_DPS_Athena_VR",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_DPS_Athena_SR.WID_Assault_Paprika_DPS_Athena_SR",

            // enforcer ar
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Infantry_Athena_SR.WID_Assault_Paprika_Infantry_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Infantry_Athena_R.WID_Assault_Paprika_Infantry_Athena_R",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Infantry_Athena_VR.WID_Assault_Paprika_Infantry_Athena_VR",

            // burst smg
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_Burst_Athena_VR.WID_SMG_Paprika_Burst_Athena_VR",
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_Burst_Athena_R.WID_SMG_Paprika_Burst_Athena_R",
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_Burst_Athena_SR.WID_SMG_Paprika_Burst_Athena_SR",

            // hyper smg
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_DPS_Athena_SR.WID_SMG_Paprika_DPS_Athena_SR",
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_DPS_Athena_R.WID_SMG_Paprika_DPS_Athena_R",
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_DPS_Athena_VR.WID_SMG_Paprika_DPS_Athena_VR",
            "FortWeaponRangedItemDefinition WID_SMG_Paprika_DPS_Athena_UR_Boss.WID_SMG_Paprika_DPS_Athena_UR_Boss",

            // nemezis ar
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Heavy_Athena_SR.WID_Assault_Paprika_Heavy_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Heavy_Athena_VR.WID_Assault_Paprika_Heavy_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Heavy_Athena_R.WID_Assault_Paprika_Heavy_Athena_R",
            "FortWeaponRangedItemDefinition WID_Assault_Paprika_Heavy_Athena_UR_Boss.WID_Assault_Paprika_Heavy_Athena_UR_Boss",
        };

        inline std::vector<std::string> Second = {
            "FortWeaponRangedItemDefinition WID_Shotgun_Pump_Paprika_Athena_SR.WID_Shotgun_Pump_Paprika_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Shotgun_Pump_Paprika_Athena_VR.WID_Shotgun_Pump_Paprika_Athena_VR",
            "FortWeaponRangedItemDefinition WID_Shotgun_Pump_Paprika_Athena_R.WID_Shotgun_Pump_Paprika_Athena_R",
            "FortWeaponRangedItemDefinition WID_Shotgun_Pump_Paprika_Athena_UR_Boss.WID_Shotgun_Pump_Paprika_Athena_UR_Boss",

            "FortWeaponRangedItemDefinition WID_Shotgun_Auto_Paprika_Athena_SR.WID_Shotgun_Auto_Paprika_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Shotgun_Auto_Paprika_Athena_UR_Boss.WID_Shotgun_Auto_Paprika_Athena_UR_Boss",
            "FortWeaponRangedItemDefinition WID_Shotgun_Auto_Paprika_Athena_VR.WID_Shotgun_Auto_Paprika_Athena_VR",
            "FortWeaponRangedItemDefinition WID_Shotgun_Auto_Paprika_Athena_R.WID_Shotgun_Auto_Paprika_Athena_R",
        };

        inline std::vector < std::string > Third = {
            "FortWeaponRangedItemDefinition WID_Sniper_Paprika_Athena_SR.WID_Sniper_Paprika_Athena_SR",
            "FortWeaponRangedItemDefinition WID_Sniper_Paprika_Athena_VR.WID_Sniper_Paprika_Athena_VR",
        };
    }

    inline std::vector<std::vector<std::string>> Attachments = {
        //scopes or idfk
        {
            "FortWeaponModItemDefinitionOptic WMOID_Optic_P2X.WMOID_Optic_P2X",
            "FortWeaponModItemDefinitionOptic WMOID_Optic_Sniper.WMOID_Optic_Sniper",
            "FortWeaponModItemDefinitionOptic WMOID_Optic_RedDot.WMOID_Optic_RedDot",
            "FortWeaponModItemDefinitionOptic WMOID_Optic_Holo.WMOID_Optic_Holo"
        },
        //ammo thingy idk how to spell it :smiley:
        {
            "FortWeaponModItemDefinitionMagazine WMID_Magazine_Drum.WMID_Magazine_Drum",
            "FortWeaponModItemDefinitionMagazine WMID_Magazine_Speed.WMID_Magazine_Speed"
        },
        //barrel or wtv you call it
        {
            "FortWeaponModItemDefinition WMID_Barrel_MuzzleBrake.WMID_Barrel_MuzzleBrake",
            "FortWeaponModItemDefinition WMID_Barrel_Suppressor.WMID_Barrel_Suppressor"
        },
        //grip
        {
            "FortWeaponModItemDefinition WMID_Underbarrel_Laser.WMID_Underbarrel_Laser",
            "FortWeaponModItemDefinition WMID_Underbarrel_VertForegrip.WMID_Underbarrel_VertForegrip",
            "FortWeaponModItemDefinition WMID_Underbarrel_AngledForegrip.WMID_Underbarrel_AngledForegrip"
        }
    };
}
