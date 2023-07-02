// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Animation/AnimationAsset.h"
#include "Components/LRLineTrace.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "LRWeapon.generated.h"

class ALRMagazine;
USTRUCT()
struct FLRWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere)
		UStaticMesh* AmmoMesh;

	UPROPERTY(EditAnywhere)
		UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
		TArray<UAnimationAsset*> ReloadAnimation;

	UPROPERTY(EditAnywhere)
		TArray<TObjectPtr<UParticleSystem>> WeaponFXs;

	UPROPERTY(EditAnywhere)
		FString SupportedMagazineName;
};
UCLASS()
class WEAPONS_API ALRWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ALRWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties)
		TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditAnywhere, Category = Utilities)
		TObjectPtr<UDataTable> WeaponDataTable;

	FLRWeaponData* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utilities)
		TSubclassOf<ALRMagazine> WeaponMagazine_Class;

	UPROPERTY(Replicated)
		TObjectPtr<ALRMagazine> CurrentMagazine;

	UPROPERTY(Replicated)
		TArray<TObjectPtr<ALRMagazine>> ExtraMagazines;

	FName MagazineName;

	UAnimationAsset* FireAnimation;

	TArray<UAnimationAsset*> ReloadAnimation;

	TArray<TObjectPtr<UParticleSystem>> WeaponFXs;

	TObjectPtr<ULRLineTrace> LineTraceComponent;

	/**Ability System Component**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class ULRWeaponASC* AbilitySystemComponent;
	
	bool bAttributesInitialized;
	
	/**Attribute Set**/
	UPROPERTY()
		class ULRWeaponAttributeSet* AttributeSet;

protected:
	//Ability System Components
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void  InitializeAttributes();
	virtual void  GiveAbilities();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TSubclassOf<class UGameplayEffect> DamageAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon|Abilities")
		TArray<TSubclassOf<class ULRWeaponGameplayAbility>> DefaultAbilities;
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetupWeapon();
	bool Server_SetupWeapon_Validate();
	void Server_SetupWeapon_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SpawnMagazines(int Amount);
	bool Server_SpawnMagazines_Validate(int Amount);
	void Server_SpawnMagazines_Implementation(int Amount);

public:	
	FHitResult Fire();
	FHitResult Fire(FVector End);
	FHitResult Fire(FHitResult HitResult);
	bool CanReload();
	void Reload();
	void SetupWeapon();
	void AddMagazine(ALRMagazine* Magazine);
};
