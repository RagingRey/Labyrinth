// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/LRLineTrace.h"
#include "Components/LRPlayerStats.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/LRWeapon.h"
#include "LRCharacter.generated.h"

UCLASS()
class LABYRINTH_API ALRCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ReloadAction;

public:
	ALRCharacter();
	
protected:
	UPROPERTY(BlueprintReadOnly)
		bool bIsJumping;

	UPROPERTY(BlueprintReadOnly)
		bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly)
		bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Aim)
		bool bIsAiming;

	UPROPERTY(Replicated)
		float PlayerPitch;

	FVector CameraDefaultLocation;
	FVector CameraAimLocation;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Animations)
		UAnimationAsset* DeathAnimation;

	UPROPERTY(Replicated)
		TObjectPtr<ALRWeapon> Weapon;

	//Player's Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		TObjectPtr<ULRLineTrace> LineTraceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerComponent)
		TObjectPtr<ULRPlayerStats> PlayerStatComponent;

protected:
	virtual void BeginPlay() override;

	//Player Movements
	void Move(const FInputActionValue& Value);

	void Sprint();
	void StopSprinting();

	void Crouch();
	void StopCrouching();

	void Look(const FInputActionValue& Value);

	void Jump();

	void StopJumping();

	//Interaction
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Interact(FVector EndLocation);
	bool Server_Interact_Validate(FVector EndLocation);
	void Server_Interact_Implementation(FVector EndLocation);

	//Attack Functionality
	void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Attack(FHitResult HitResult);
	bool Server_Attack_Validate(FHitResult HitResult);
	void Server_Attack_Implementation(FHitResult HitResult);

	//Reload Functionality
	void Reload();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reload();
	bool Server_Reload_Validate();
	void Server_Reload_Implementation();

	//Aim Functionality
	void Aim();
	void StopAiming();

	UFUNCTION()
		void OnRep_Aim();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Aim(bool Aim);
	bool Server_Aim_Validate(bool Aim);
	void Server_Aim_Implementation(bool Aim);

	UFUNCTION(BlueprintCallable)
		float GetPlayerPitch();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_UpdatePitch(float Pitch);
	bool Server_UpdatePitch_Validate(float Pitch);
	void Server_UpdatePitch_Implementation(float Pitch);

	void Die();

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void Multi_Die();
	bool Multi_Die_Validate();
	void Multi_Die_Implementation();

	void AddWeapon(ALRWeapon* NewWeapon);

	UFUNCTION(BlueprintPure)
		float GetHealth() const { return PlayerStatComponent->GetHealth(); }

	UFUNCTION(BlueprintPure)
		float GetArmor() const { return PlayerStatComponent->GetArmor(); }

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
