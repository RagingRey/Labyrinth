// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LRWeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class WEAPONS_API ULRWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ULRWeaponAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	//void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute,
	//	const float NexMaxValue, const FGameplayAttribute& AffectedAttributeProp);
	
	UPROPERTY(BlueprintReadOnly, Category = Attributes, ReplicatedUsing = OnRep_CurrentAmmo)
	FGameplayAttributeData CurrentAmmo;
	ATTRIBUTE_ACCESSORS(ULRWeaponAttributeSet, CurrentAmmo)

	UFUNCTION()
	virtual void OnRep_CurrentAmmo(const FGameplayAttributeData& OldCurrentAmmo);
	
	UPROPERTY(BlueprintReadOnly, Category = Attributes, ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(ULRWeaponAttributeSet, MaxAmmo)

	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo);

	UPROPERTY(BlueprintReadOnly, Category = Attributes, ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ULRWeaponAttributeSet, Damage)

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);
};
