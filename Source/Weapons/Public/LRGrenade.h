// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "LRGrenade.generated.h"

UENUM()
enum class EGrenadeType
{
	Frag,
	Smoke
};

UCLASS()
class WEAPONS_API ALRGrenade : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALRGrenade();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		EGrenadeType GrenadeType = EGrenadeType::Frag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
		TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
		TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		int CountDown;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		TObjectPtr<UParticleSystem> ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		TObjectPtr<USoundBase> ExplosionSound;

	FTimerHandle ExplosionHandle;

	
	//Ability System Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class ULRWeaponASC* AbilitySystemComponent;
	
	bool bAttributesInitialized;
	
	UPROPERTY()
		class ULRWeaponAttributeSet* AttributeSet;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void  InitializeAttributes();
	virtual void  GiveAbilities();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TSubclassOf<class UGameplayEffect> DamageAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TArray<TSubclassOf<class ULRWeaponGameplayAbility>> DefaultAbilities;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Launch();
	void Explode();

	void OnOverlap();
};
