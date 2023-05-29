// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Components/LRPlayerStats.h"
#include "GameFramework/Actor.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
ULRPlayerStats::ULRPlayerStats()
{
	Health = 80;
	Armor = 40;
}


void ULRPlayerStats::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void ULRPlayerStats::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ULRPlayerStats, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ULRPlayerStats, Armor, COND_OwnerOnly);
}


void ULRPlayerStats::LowerHealth(float Damage)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_LowerHealth(Damage);
	}
	else
	{
		Health -= Damage;

		if (Health <= 0.0f)
			Health = 0.0f;
	}
}

bool ULRPlayerStats::Server_LowerHealth_Validate(float Damage)
{
	return true;
}

void ULRPlayerStats::Server_LowerHealth_Implementation(float Damage)
{
	if(GetOwnerRole() == ROLE_Authority)
		Health -= Damage;
}

void ULRPlayerStats::LowerArmor(float Damage)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_LowerArmor(Damage);
	}
	else
	{
		Armor -= Damage;

		if (Armor <= 0.0f)
		{
			Armor = 0.0f;
			LowerHealth(Damage);
		}
	}
}

bool ULRPlayerStats::Server_LowerArmor_Validate(float Damage)
{
	return (Armor > 0);
}

void ULRPlayerStats::Server_LowerArmor_Implementation(float Damage)
{
	Armor -= Damage;
}

