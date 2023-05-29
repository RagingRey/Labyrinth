// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Components/LRLineTrace.h"

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

ULRLineTrace::ULRLineTrace()
{
	
}

FHitResult ULRLineTrace::LineTraceSingle(FVector StartLocation, FVector EndLocation, bool bShowDebugLine)
{
	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByObjectType(OUT HitResult, StartLocation, EndLocation, CollisionObjectQueryParams, CollisionQueryParams))
	{
		if (bShowDebugLine)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.TraceEnd, FColor::Red, false, 3.0f, 0, 5.0f);
		}

		return HitResult;
	}
	else
	{
		if (bShowDebugLine)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.TraceEnd, FColor::Red, false, 3.0f, 0, 5.0f);
		}

		return HitResult;
	}
}
