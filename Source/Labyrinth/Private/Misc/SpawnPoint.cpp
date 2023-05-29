// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Misc/SpawnPoint.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	this->RootComponent = StaticMesh;
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);
}