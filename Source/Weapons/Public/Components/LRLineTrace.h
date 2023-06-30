// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "LRLineTrace.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONS_API ULRLineTrace : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRLineTrace();

	FHitResult LineTraceSingle(FVector StartLocation, FVector EndLocation, bool bShowDebugLine);
		
};
