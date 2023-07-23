// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemInterface.h"
#include "LRGrenade.h"
#include "Abilities/LRCharacterAttributeSet.h"
#include "Components/LRInventory.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Labyrinth/Labyrinth.h"
#include "Weapons/Public/LRWeapon.h"
#include "Weapons/Public/Components/LRLineTrace.h"
#include "LRCharacter.generated.h"

UCLASS()
class LABYRINTH_API ALRCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	//Inputs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* LabyrinthMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	//Weapon Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* GrenadeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ReloadAction;

	//Widget Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WidgetMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InventoryAction;

	/**Ability System Component**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class ULRCharacterASC* AbilitySystemComponent;
	
	bool bAttributesInitialized;
	
	/**Attribute Set**/
	UPROPERTY()
		class ULRCharacterAttributeSet* AttributeSet;

public:
	ALRCharacter();

	//Ability System Components
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	 virtual void  InitializeAttributes();
	 virtual void  GiveAbilities();
	
	 virtual void PossessedBy(AController* NewController) override;
	 virtual void OnRep_PlayerState() override;
	
	 UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character|Abilities")
	 	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	 UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character|Abilities")
	 	TArray<TSubclassOf<class ULRCharacterGameplayAbility>> DefaultAbilities;
	
protected:
	//Weapons
	UPROPERTY(Replicated)
		TObjectPtr<ALRWeapon> Weapon;

	int Grenades;
	UPROPERTY(EditDefaultsOnly, Category = "Character|Weapon")
		TSubclassOf<ALRGrenade> Grenade_Class;


	//Player's Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		TObjectPtr<ULRLineTrace> LineTraceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		TObjectPtr<ULRInventory> Inventory;

	//Inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<UUserWidget> InventoryUI_Class;
	TObjectPtr<UUserWidget> InventoryUI;
	void ToggleInventory();
	
	virtual void BeginPlay() override;

	//Player Movements
	void Move(const FInputActionValue& Value);

	UPROPERTY(BlueprintReadOnly)
		bool bIsSprinting;
	void Sprint();
	void StopSprinting();

	UPROPERTY(BlueprintReadOnly)
		bool bIsCrouching;
	void Crouch();
	void StopCrouching();

	void Look(const FInputActionValue& Value);

	UPROPERTY(BlueprintReadOnly)
		bool bIsJumping;
	virtual void Jump() override;

	virtual void StopJumping() override;

	//Interaction
	void Interact(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID);
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_Interact();

	//Attack Functionality
	void Attack(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID);
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void FireWeapon();

	//Attack Functionality
	void ThrowGrenade(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID);
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void SpawnGrenade();

	//Reload Functionality
	void Reload();

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_Reload();
	bool Server_Reload_Validate();
	void Server_Reload_Implementation();

	//Aim Functionality
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Aim)
		bool bIsAiming;

	FVector CameraDefaultLocation;
	FVector CameraAimLocation;
	void Aim();
	void StopAiming();

	UFUNCTION()
		void OnRep_Aim();

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_Aim(bool Aim);
	bool Server_Aim_Validate(bool Aim);
	void Server_Aim_Implementation(bool Aim);

	UPROPERTY(Replicated)
		float PlayerPitch;
	
	UFUNCTION(BlueprintCallable)
		float GetPlayerPitch();

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_UpdatePitch(float Pitch);
	bool Server_UpdatePitch_Validate(float Pitch);
	void Server_UpdatePitch_Implementation(float Pitch);


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Animations)
		UAnimationAsset* DeathAnimation;
	void Die();

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void Multi_Die();
	bool Multi_Die_Validate();
	void Multi_Die_Implementation();
	
	UFUNCTION(BlueprintPure)
		float GetHealth() const;

	UFUNCTION(BlueprintPure)
		float GetArmor() const;

	void AddWeapon(ALRWeapon* NewWeapon);
	void AddGrenade(const ALRGrenade* NewGrenade);
	
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	FORCEINLINE ALRWeapon* GetWeapon() const { return Weapon; }
};
