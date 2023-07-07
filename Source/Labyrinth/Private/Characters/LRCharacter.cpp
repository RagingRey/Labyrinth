// Labyrinth, copyright Rage Games 2023, all rights reserved.

#include "Characters/LRCharacter.h"

#include "Engine.h"
#include "EngineMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Abilities/LRCharacterASC.h"
#include "Abilities/LRCharacterGameplayAbility.h"
#include "LRGameMode.h"
#include "Weapons/Public/LRGrenade.h"
#include "Kismet/KismetMathLibrary.h"
#include "Labyrinth/Labyrinth.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Public/LRMagazine.h"

//Sets Default values on Instantiation of the Class
ALRCharacter::ALRCharacter()
{
	this->GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Setup Player Movement according to FollowCamera's rotation and other attributes of the player's abilities
	this->GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	this->GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	this->GetCharacterMovement()->JumpZVelocity = 500.f;
	this->GetCharacterMovement()->AirControl = 0.3f;
	this->GetCharacterMovement()->MaxWalkSpeed = 500.0;
	this->GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	this->GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Components
	LineTraceComponent = CreateDefaultSubobject<ULRLineTrace>(TEXT("Line Trace Component"));

	AbilitySystemComponent = CreateDefaultSubobject<ULRCharacterASC>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<ULRCharacterAttributeSet>("Attributes");
	
	bAttributesInitialized = false;

	//Defined Variables
	bIsJumping = false;
	bIsSprinting = false;

	Grenades = 0;
}

UAbilitySystemComponent* ALRCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRCharacter::InitializeAttributes()
{
	check(AbilitySystemComponent);
	
	if(AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle  = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	bAttributesInitialized = true;
}

void ALRCharacter::GiveAbilities()
{
	if(HasAuthority() && AbilitySystemComponent)
	{
		for (const TSubclassOf<ULRCharacterGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}
	}
}

void ALRCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GiveAbilities();
}

void ALRCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
}

// Called when the game starts or when spawned
void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	if (APlayerController* PlayerController = Cast<APlayerController>(this->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(LabyrinthMappingContext, 0);
			Subsystem->AddMappingContext(WeaponMappingContext, 1);
		}
	}

	CameraDefaultLocation = FollowCamera->GetComponentLocation();
	CameraAimLocation = FollowCamera->GetComponentLocation() + FVector(100, 0, 0);
}

void ALRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALRCharacter, bIsAiming);
	DOREPLIFETIME(ALRCharacter, Weapon);
	DOREPLIFETIME_CONDITION(ALRCharacter, PlayerPitch, COND_SkipOwner);
}

// Called every frame
void ALRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ALRCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ALRCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALRCharacter::Move);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ALRCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALRCharacter::StopSprinting);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ALRCharacter::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ALRCharacter::StopCrouching);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALRCharacter::Look);

		//Fire Weapon
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ALRCharacter::Attack, true, ELabyrinthAbilityInputID::Attack);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ALRCharacter::Attack, false, ELabyrinthAbilityInputID::Attack);

		//Throw Grenade
		EnhancedInputComponent->BindAction(GrenadeAction, ETriggerEvent::Triggered, this, &ALRCharacter::ThrowGrenade, true, ELabyrinthAbilityInputID::Grenade);
		EnhancedInputComponent->BindAction(GrenadeAction, ETriggerEvent::Triggered, this, &ALRCharacter::ThrowGrenade, false, ELabyrinthAbilityInputID::Grenade);

		//Aim Weapon
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ALRCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ALRCharacter::StopAiming);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALRCharacter::Interact, true, ELabyrinthAbilityInputID::Interact);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALRCharacter::Interact, false, ELabyrinthAbilityInputID::Interact);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ALRCharacter::Reload);
	}
}

void ALRCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (this->GetController())
	{
		const float YawValue = GetControlRotation().Yaw;
		const FRotator YawRotation = FRotator(0, YawValue, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if(!bIsSprinting)
		{
			MovementVector.X /= 2;
			MovementVector.Y /= 2;
		}

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALRCharacter::Sprint()
{
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		bIsCrouching = false;
	}
}

void ALRCharacter::StopSprinting()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
	}
}

void ALRCharacter::Crouch()
{
	if (!bIsCrouching && !GetCharacterMovement()->IsFalling())
	{
		bIsCrouching = true;
		bIsSprinting = false;
		GetCharacterMovement()->bWantsToCrouch = true;
	}
}

void ALRCharacter::StopCrouching()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}

void ALRCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (this->GetController())
	{
		AddControllerPitchInput(LookAxisVector.Y);
		AddControllerYawInput(LookAxisVector.X);
	}

	FRotator NormalizedRotation = (this->GetControlRotation() - this->GetActorRotation()).GetNormalized();
	PlayerPitch = NormalizedRotation.Pitch;

	Server_UpdatePitch(PlayerPitch);
}

float ALRCharacter::GetPlayerPitch()
{
	return PlayerPitch * -1;
}

bool ALRCharacter::Server_UpdatePitch_Validate(float Pitch)
{
	return true;
}

void ALRCharacter::Server_UpdatePitch_Implementation(float Pitch)
{
	PlayerPitch = Pitch;
}

void ALRCharacter::Jump()
{
	ACharacter::Jump();
	bIsJumping = true;
}

void ALRCharacter::StopJumping()
{
	ACharacter::StopJumping();
	bIsJumping = false;
}

void ALRCharacter::Interact(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID)
{
	if (!AbilitySystemComponent) return;

	if(bPressed)
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	else
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
}

void ALRCharacter::Server_Interact_Implementation()
{
	if (HasAuthority())
	{
		FVector CamStartLocation = FollowCamera->GetComponentLocation();
		FVector CamEndLocation = CamStartLocation + (FollowCamera->GetForwardVector() * 2000.0f);

		FVector StartLocation = this->GetMesh()->GetBoneLocation("head");
		FVector ImpactPoint = LineTraceComponent->LineTraceSingle(CamStartLocation, CamEndLocation, false).ImpactPoint;
		FVector EndLocation;

		if(!ImpactPoint.IsZero())
		{
			EndLocation = StartLocation + UKismetMathLibrary::FindLookAtRotation(StartLocation, ImpactPoint).Vector() * 2000.0f;
		}
		else
		{
			EndLocation = StartLocation + UKismetMathLibrary::FindLookAtRotation(StartLocation, CamEndLocation).Vector() * 2000.0f;
		}

		FHitResult HitResult = LineTraceComponent->LineTraceSingle(StartLocation, EndLocation, true);

		if(ALRWeapon* InteractedWeapon = Cast<ALRWeapon>(HitResult.GetActor()))
		{
			AddWeapon(InteractedWeapon);
		}
		else if(ALRMagazine* InteractedMagazine = Cast<ALRMagazine>(HitResult.GetActor()))
		{
			if(Weapon)
				Weapon->AddMagazine(InteractedMagazine);				
		}
		else if(ALRGrenade* InteractedGrenade = Cast<ALRGrenade>(HitResult.GetActor()))
		{
			AddGrenade(InteractedGrenade);
		}
	}
}

void ALRCharacter::Attack(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID)
{
	if (!AbilitySystemComponent) return;

	if(bPressed)
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	else
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
}

void ALRCharacter::ThrowGrenade(bool bPressed, const ELabyrinthAbilityInputID AbilityInputID)
{
	if (!AbilitySystemComponent) return;

	if(bPressed)
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	else
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
}

void ALRCharacter::FireWeapon_Implementation()
{
	if(HasAuthority())
	{
		if(Weapon)
		{
			if(bIsAiming)
			{
				const FVector StartLocation = FollowCamera->GetComponentLocation();
				const FVector EndLocation = StartLocation + (FollowCamera->GetForwardVector() * 3500.0f);
				const FVector ImpactPoint = LineTraceComponent->LineTraceSingle(StartLocation, EndLocation, false).ImpactPoint;
		
				if(!ImpactPoint.IsZero())
				{
					Weapon->Fire(ImpactPoint);
				}
				else
				{
					Weapon->Fire(EndLocation);
				}
			}
			else
			{
				Weapon->Fire();
			}
		}
	}
}

void ALRCharacter::SpawnGrenade_Implementation()
{
	if(HasAuthority())
	{
		if(Grenades > 0 && Grenade_Class)
		{
			const FRotator GrenadeSpawnRotation = this->GetControlRotation();
			const FVector GrenadeSpawnLocation = this->GetActorLocation() + GrenadeSpawnRotation.RotateVector(FVector(300, 0, 0));
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			SpawnParameters.Owner = this;

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("Semi-Worked"));

			ALRGrenade* Grenade = GetWorld()->SpawnActor<ALRGrenade>(Grenade_Class, GrenadeSpawnLocation, this->GetActorRotation(), SpawnParameters);
			Grenades--;
		}	
	}
}

float ALRCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetHealth() <= 0.0f)
		Die();

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void ALRCharacter::Reload()
{
	if(Weapon->CanReload())
	{
		Server_Reload();
	}
}

bool ALRCharacter::Server_Reload_Validate()
{
	return true;
}

void ALRCharacter::Server_Reload_Implementation()
{
	Weapon->Reload();
}

void ALRCharacter::Aim()
{
	if(Weapon)
		Server_Aim(true);

	//Camera Movement
	/*if (!CameraAimLocation.IsZero() && !CameraDefaultLocation.IsZero())
	{
		FVector NewLocation = UKismetMathLibrary::VInterpTo(FollowCamera->GetComponentLocation(), CameraAimLocation, GetWorld()->DeltaTimeSeconds, 10.0f);
		FollowCamera->SetRelativeLocation(NewLocation);
	}*/
}

void ALRCharacter::StopAiming()
{
	if (Weapon)
		Server_Aim(false);

	//Camera Movement
	/*if (!CameraAimLocation.IsZero() && !CameraDefaultLocation.IsZero())
	{
		FVector NewLocation = UKismetMathLibrary::VInterpTo(CameraAimLocation, CameraDefaultLocation, GetWorld()->DeltaTimeSeconds, 10.0f);
		FollowCamera->SetRelativeLocation(NewLocation);
	}*/
}

void ALRCharacter::OnRep_Aim()
{
	this->bUseControllerRotationYaw = bIsAiming;
}

bool ALRCharacter::Server_Aim_Validate(bool Aim)
{
	return true;
}

void ALRCharacter::Server_Aim_Implementation(bool Aim)
{
	bIsAiming = Aim;
	OnRep_Aim();
}

void ALRCharacter::Die()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Called"));

		Multi_Die();

		AGameModeBase* CurrentGameMode = GetWorld()->GetAuthGameMode();
		if (ALRGameMode* GameMode = Cast<ALRGameMode>(CurrentGameMode))
		{
			GameMode->Respawn(this->GetController());
		}
	}
}

bool ALRCharacter::Multi_Die_Validate()
{
	return true;
}

void ALRCharacter::Multi_Die_Implementation()
{
	this->GetCharacterMovement()->DisableMovement();
	this->GetMesh()->PlayAnimation(DeathAnimation, false);
	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//this->GetMesh()->SetSimulatePhysics(true);
}

void ALRCharacter::AddWeapon(ALRWeapon* NewWeapon)
{
	if(!Weapon)
	{
		Weapon = NewWeapon;
		Weapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("GunPoint"));
		Weapon->SetOwner(this);
	}
}

void ALRCharacter::AddGrenade(const ALRGrenade* NewGrenade)
{
	if(NewGrenade)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("Semi-Worked"));
		Grenades++;
	}
}

float ALRCharacter::GetHealth() const
{
	if(AttributeSet) return AttributeSet->GetHealth();

	return 1.f;
}

float ALRCharacter::GetArmor() const
{
	if(AttributeSet) return AttributeSet->GetArmor();

	return 1.f;
}
