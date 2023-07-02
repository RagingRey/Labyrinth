// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Abilities/LRWeaponAttributeSet.h"

#include "Net/UnrealNetwork.h"

ULRWeaponAttributeSet::ULRWeaponAttributeSet()
{
	
}

void ULRWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULRWeaponAttributeSet, CurrentAmmo, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRWeaponAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRWeaponAttributeSet, Damage, COND_None, REPNOTIFY_Always)
}

void ULRWeaponAttributeSet::OnRep_CurrentAmmo(const FGameplayAttributeData& OldCurrentAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRWeaponAttributeSet, CurrentAmmo, OldCurrentAmmo);
}

void ULRWeaponAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRWeaponAttributeSet, MaxAmmo, OldMaxAmmo);
}

void ULRWeaponAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRWeaponAttributeSet, Damage, OldDamage);
}
