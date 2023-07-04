// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Abilities/LRCharacterAttributeSet.h"

#include "GameplayEffectExtension.h"
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

void ULRCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float DeltaValue = 0.f;
		if(Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
			DeltaValue = Data.EvaluatedData.Magnitude;

		if(DeltaValue != 0.f)
		{
			if(GetArmor() > 0.f)
			{
				const float OldArmor = GetArmor();
				const float OldHealth = GetHealth();
				SetArmor(FMath::Clamp((OldArmor + DeltaValue), 0.f, GetMaxHealth()));
				SetHealth(FMath::Clamp((OldHealth - DeltaValue), 0.f, GetMaxHealth()));	
			}
		}
	}
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


