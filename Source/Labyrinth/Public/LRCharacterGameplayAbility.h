// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Labyrinth/Labyrinth.h"
#include "LRCharacterGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ULRCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		ELabyrinthAbilityInputID AbilityInputID = ELabyrinthAbilityInputID::None;
};
