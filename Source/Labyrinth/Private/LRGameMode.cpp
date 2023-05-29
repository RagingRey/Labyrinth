// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "LRGameMode.h"

#include "Kismet/GameplayStatics.h"

ALRGameMode::ALRGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	RespawnCountDown = 3.0f;
}

void ALRGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(SpawnPoint_Class)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPoint_Class, SpawnPointActors);

		for(AActor* Actor: SpawnPointActors)
		{
			SpawnPoints.Add(Cast<ASpawnPoint>(Actor));
		}
	}
}

void ALRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(AController* Controller = Cast<AController>(NewPlayer))
	{
		Spawn(Controller);
	}
}

FVector ALRGameMode::GetSpawnLocation() const
{
	if(SpawnPoints.Num()) //test this
	{
		const int Slot = FMath::RandRange(0, SpawnPoints.Num() - 1);
		return SpawnPoints[Slot]->GetActorLocation();
	}

	return DefaultSpawnLocation;
}

void ALRGameMode::Spawn(AController* Controller)
{
	if(Controller)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if(APawn* Player = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, GetSpawnLocation(), FRotator::ZeroRotator, SpawnParameters))
		{
			Controller->Possess(Player);
		}
	}
}

void ALRGameMode::Respawn(AController* Controller)
{
	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate.BindUFunction(this, FName("Spawn"), Controller);
	GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, RespawnCountDown, false);
}
