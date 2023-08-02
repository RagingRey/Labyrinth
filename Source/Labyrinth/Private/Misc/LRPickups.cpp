// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Misc/LRPickups.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ALRPickups::ALRPickups()
{
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

void ALRPickups::UseItem()
{
	if(PickupType == EPickupType::FirstAid)
	{
		
	}
}

void ALRPickups::Picked()
{
	bPicked = true;
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
}