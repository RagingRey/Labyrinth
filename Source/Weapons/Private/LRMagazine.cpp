// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "LRMagazine.h"

//#include "Characters/LRCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALRMagazine::ALRMagazine()
{
	MagazineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));

	this->RootComponent = MagazineMeshComponent;
	MagazineMeshComponent->SetCollisionProfileName(FName("BlockAll"));
}

void ALRMagazine::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	SetUpMagazine("AssaultRifle_Pickup");
}

void ALRMagazine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALRMagazine, MagazineAmmo);
	DOREPLIFETIME(ALRMagazine, bMagazineInUse);
}

void ALRMagazine::SetUpMagazine(FName MagazineName)
{
	if (MagazineDataTable)
	{
		FString ContextString = "Magazine for the weapon isn't in the database";
		MagazineData = MagazineDataTable->FindRow<FLRMagazineData>(MagazineName, ContextString);

		if (MagazineData)
		{
			MagazineMeshComponent->SetStaticMesh(MagazineData->MagazineStaticMesh);
			MagazineMeshComponent->SetWorldScale3D(FVector(3));
			MagazineAmmo = MagazineData->MagazineCapacity;
		}
	}
}

void ALRMagazine::PickUp()
{
	bMagazineInUse = true;
	OnRep_MagazineUsed();
}

void ALRMagazine::OnRep_MagazineUsed()
{
	this->MagazineMeshComponent->SetHiddenInGame(bMagazineInUse);
	this->SetActorEnableCollision(!bMagazineInUse);
}
