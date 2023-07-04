// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Weapons/Weapons.h"
#include "LRWeaponGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONS_API ULRWeaponGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		ELRWeaponAbilityInputID AbilityInputID = ELRWeaponAbilityInputID::None;
};
