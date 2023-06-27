// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "LRCharacterAttributeSet.h"

#include "Net/UnrealNetwork.h"

ULRCharacterAttributeSet::ULRCharacterAttributeSet()
{
	
}

void ULRCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULRCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRCharacterAttributeSet, Armor, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ULRCharacterAttributeSet, AttackPower, COND_None, REPNOTIFY_Always)
}

void ULRCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRCharacterAttributeSet, Health, OldHealth);
}

void ULRCharacterAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRCharacterAttributeSet, Armor, OldArmor);
}

void ULRCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void ULRCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRCharacterAttributeSet, Stamina, OldStamina);
}

void ULRCharacterAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULRCharacterAttributeSet, AttackPower, OldAttackPower);
}


