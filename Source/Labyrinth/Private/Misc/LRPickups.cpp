// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Misc/LRPickups.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALRPickups::ALRPickups()
{
	// static ConstructorHelpers::FObjectFinder<UGameplayEffect> HealthEffect(TEXT("/Game/Blueprints/Pickups/Abilities/GE_FirstAid"));
	// if(HealthEffect.Object)
	// {
	// 	HealthAttributeEffect = HealthEffect.Object;
	// }
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	this->RootComponent = MeshComponent;
	
	Icon = CreateDefaultSubobject<UTexture2D>("Icon");
	
	bPicked = false;
}

void ALRPickups::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
}

void ALRPickups::UseItem_Implementation()
{
	if(HasAuthority())
	{
		if(PickupType == EPickupType::FirstAid)
		{
			if(ItemEffect && UKismetSystemLibrary::DoesImplementInterface(this->GetOwner(), UAbilitySystemInterface::StaticClass()))
			{
				if(UAbilitySystemComponent* Component = Cast<IAbilitySystemInterface>(this->GetOwner())->GetAbilitySystemComponent())
				{
					Component->ApplyGameplayEffectToSelf(ItemEffect.GetDefaultObject(), 1.f, FGameplayEffectContextHandle());
				}
			}
		}
	}
}

void ALRPickups::Picked()
{
	if(HasAuthority())
	{
		bPicked = true;
		OnRep_Picked();		
	}
}

void ALRPickups::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALRPickups, bPicked);
}

void ALRPickups::OnRep_Picked()
{
	this->MeshComponent->SetHiddenInGame(bPicked);
	this->SetActorEnableCollision(!bPicked);
	Destroy();
}