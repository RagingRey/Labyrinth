// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class LABYRINTH_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "S | Mesh")
		TObjectPtr<UStaticMeshComponent> StaticMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
