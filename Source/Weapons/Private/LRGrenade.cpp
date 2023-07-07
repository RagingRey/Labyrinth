// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "LRGrenade.h"

#include "AbilitySystemInterface.h"
#include "Abilities/LRWeaponASC.h"
#include "Abilities/LRWeaponAttributeSet.h"
#include "Abilities/LRWeaponGameplayAbility.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ALRGrenade::ALRGrenade()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	this->RootComponent = MeshComponent;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereCollision->SetupAttachment(MeshComponent);
	SphereCollision->SetSphereRadius(1300.0f);

	CountDown = 2.0f;

	AbilitySystemComponent = CreateDefaultSubobject<ULRWeaponASC>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<ULRWeaponAttributeSet>("Attributes");

	bAttributesInitialized = false;
}

UAbilitySystemComponent* ALRGrenade::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRGrenade::InitializeAttributes()
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

void ALRGrenade::GiveAbilities()
{
	if(HasAuthority() && AbilitySystemComponent)
	{
		for (const TSubclassOf<ULRWeaponGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}
	}
}

// Called when the game starts or when spawned
void ALRGrenade::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	InitializeAttributes();

	GiveAbilities();
	
	Launch();
}

void ALRGrenade::Launch()
{
	if(GetOwner())
	{
		FRotator Rotation = Cast<ACharacter>(GetOwner())->GetControlRotation();
		float Impulse = 5000.f;
		FVector Direction = UKismetMathLibrary::GetForwardVector(Rotation);

		this->MeshComponent->SetSimulatePhysics(true);
		this->MeshComponent->AddImpulse(Impulse * Direction);

		GetWorldTimerManager().SetTimer(ExplosionHandle, this, &ALRGrenade::Explode, CountDown);
	}
}

void ALRGrenade::Explode()
{
	if(GrenadeType == EGrenadeType::Frag)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, MeshComponent->GetComponentLocation(), FRotator::ZeroRotator, FVector(6));
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, MeshComponent->GetComponentLocation());
		OnOverlap();
		Destroy();
	}
	else
	{
		UParticleSystemComponent* Emitter = UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, MeshComponent->GetComponentLocation(), FRotator::ZeroRotator, FVector(6));
		UAudioComponent* Sound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, MeshComponent->GetComponentLocation());

		FTimerDelegate Delegate;
		FTimerHandle SmokeGrenade_Handle;

		Delegate.BindLambda([&]
			{
				this->ExplosionFX->FinishDestroy();
				this->ExplosionSound->FinishDestroy();
				Destroy();
			});

		GetWorldTimerManager().SetTimer(SmokeGrenade_Handle, Delegate, 8.0f, false);
	}
}

void ALRGrenade::OnOverlap()
{
	TArray<TObjectPtr<AActor>> OverlappingActors;
	SphereCollision->GetOverlappingActors(OverlappingActors);

	for(int i {0}; i < OverlappingActors.Num(); i++)
	{
		if(OverlappingActors[i] != this && UKismetSystemLibrary::DoesImplementInterface(OverlappingActors[i], UAbilitySystemInterface::StaticClass()))
		{
			AbilitySystemComponent->ApplyGameplayEffectToTarget(DamageAttributeEffect.GetDefaultObject(),
					Cast<IAbilitySystemInterface>(OverlappingActors[i])->GetAbilitySystemComponent());
		}
	}
}

