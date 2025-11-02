#ifndef ABILITIES
#define ABILITIES
#pragma once
#include "Utils.h"

inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, FPredictionKey, UGameplayAbility**, void*, const FGameplayEventData*) = decltype(InternalTryActivateAbility)(InSDKUtils::GetImageBase() + 0x724A168);
inline FGameplayAbilitySpecHandle* (*InternalGiveAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(InternalGiveAbility)(InSDKUtils::GetImageBase() + 0x72492BC);
inline __int64 (*SpecConstructor)(FGameplayAbilitySpec*, UObject*, int, int, UObject*) = decltype(SpecConstructor)(InSDKUtils::GetImageBase() + 0x728D9B4);
inline FGameplayAbilitySpecHandle (*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec, __int64) = decltype(GiveAbilityAndActivateOnce)(__int64(GetModuleHandleW(0)) + 0x7249678);

FGameplayAbilitySpec* FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
    for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->ActivatableAbilities.Items)
    {
        if (Spec.Handle.Handle == Handle.Handle)
        {
            return &Spec;
        }
    }
    return nullptr;
}

void InternalServerTryActiveAbilityHook(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
{
    
    FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySystemComponent, Handle);
    if (!Spec)
    {
        AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
        return;
    }
    const UGameplayAbility* AbilityToActivate = Spec->Ability;
    
    if (!AbilityToActivate)
    {
        return;
    }
    //std::cout << AbilityToActivate->GetFullName() << "\n";
    UGameplayAbility* InstancedAbility = nullptr;
    Spec->InputPressed = true;

    if (!InternalTryActivateAbility(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
        AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
        Spec->InputPressed = false;
        AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
    }
}

void GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UGameplayAbility* GameplayAbility)
{
    if (!GameplayAbility)
        return;
    FGameplayAbilitySpec Spec;
    SpecConstructor(&Spec, GameplayAbility, 1, -1, nullptr);
    InternalGiveAbility(AbilitySystemComponent, &Spec.Handle, Spec);
}

void GiveAbilitySet(AFortPlayerControllerAthena* PC, UFortAbilitySet* Set)
{
    if (Set)
    {
        auto PlayerState = (AFortPlayerStateAthena*)(PC->PlayerState);

        for (size_t i = 0; i < Set->GameplayAbilities.Num(); i++)
        {
            UGameplayAbility* Ability = (UGameplayAbility*) Set->GameplayAbilities[i].Get()->DefaultObject;
            
            if (Ability) { GiveAbility(PlayerState->AbilitySystemComponent, Ability); }
        }

        for (int i = 0; i < Set->GrantedGameplayEffects.Num(); ++i)
        {
            
            UClass* GameplayEffect = Set->GrantedGameplayEffects[i].GameplayEffect.Get();
            float Level = Set->GrantedGameplayEffects[i].Level;
            if (!GameplayEffect)
                continue;

            PlayerState->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GameplayEffect, Level, FGameplayEffectContextHandle());
        }
    }
}
void GiveDefaultAbilitySet(AFortPlayerControllerAthena* PC)
{
    UFortAbilitySet* DefaultAbilitySet = StaticLoadObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");
    GiveAbilitySet(PC, DefaultAbilitySet);
    UFortAbilitySet* TacticalSprintAbilitySet = StaticLoadObject<UFortAbilitySet>("/TacticalSprintGame/Gameplay/AS_TacticalSprint.AS_TacticalSprint");
    GiveAbilitySet(PC, TacticalSprintAbilitySet);
    //UFortAbilitySet* ZipFix = StaticFindObject<UFortAbilitySet>("/Ascender/Gameplay/Ascender/AS_Ascender.AS_Ascender");
    //GiveAbilitySet(PC, ZipFix);
}


#endif // !ABILITIES


