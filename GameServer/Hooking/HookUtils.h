#ifndef HOOKUTILS
#define HOOKUTILS
#include "MinHook/MinHook.h"
#include <Windows.h>
#include <iostream>

class HookUtils {
private:
    static bool ReturnFalse() {
        return false;
    }
    static bool ReturnTrue() {
        return true;
    }
public:
    static void Byte(uintptr_t Address, uint8_t byte)
    {
        DWORD oldProtection;
        if (VirtualProtect(reinterpret_cast<void*>(Address), sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtection))
        {
            *reinterpret_cast<uint8_t*>(Address) = byte;

            DWORD tempProtection;
            VirtualProtect(reinterpret_cast<void*>(Address), sizeof(uint8_t), oldProtection, &tempProtection);
        }
    }
    static void Null(uintptr_t Address)
    {
        DWORD oldProtection;
        if (VirtualProtect(reinterpret_cast<void*>(Address), sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtection))
        {
            *reinterpret_cast<uint8_t*>(Address) = 0xC3;

            DWORD tempProtection;
            VirtualProtect(reinterpret_cast<void*>(Address), sizeof(uint8_t), oldProtection, &tempProtection);
        }
    }

    static void True(uintptr_t Address) {
        Hook(Address, ReturnTrue);
    }
    static void False(uintptr_t Address) {
        Hook(Address, ReturnFalse);
    }
    static void Hook(uintptr_t Address, LPVOID Detour, LPVOID* Orig = nullptr) {
        if (MH_CreateHook((LPVOID)Address, Detour, Orig) == MH_STATUS::MH_ERROR_NOT_INITIALIZED) {
            MH_CreateHook((LPVOID)Address, Detour, Orig);
            MH_Initialize();
        }
        MH_EnableHook((LPVOID)Address);
    }
    static void VHook(void* Base, uintptr_t Index, void* Detour, void** Original = nullptr) {
        auto VTable = (*(void***)Base);
        if (!VTable)
            return;

        if (!VTable[Index])
            return;

        if (Original)
            *Original = VTable[Index];

        DWORD dwOld;
        VirtualProtect(&VTable[Index], 8, PAGE_EXECUTE_READWRITE, &dwOld);
        VTable[Index] = Detour;
        DWORD dwTemp;
        VirtualProtect(&VTable[Index], 8, dwOld, &dwTemp);
    }
};

#endif // !HOOKUTILS

#pragma once
