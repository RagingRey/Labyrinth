// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/SpawnPoint.h"
#include "LRGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALRGameMode();

protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ASpawnPoint> SpawnPoint_Class;
	TArray<AActor*> SpawnPointActors;
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere)
		FVector DefaultSpawnLocation;

	float RespawnCountDown;

protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	FVector GetSpawnLocation() const;

	UFUNCTION()
		void Spawn(AController* Controller);

public:
	void Respawn(AController* Controller);
};
