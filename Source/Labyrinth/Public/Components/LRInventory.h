// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/LRPickups.h"
#include "LRInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LABYRINTH_API ULRInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRInventory();

	void AddItem(ALRPickups* Item);
	
protected:
	TArray<ALRPickups*> InventoryItems;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FORCEINLINE TArray<ALRPickups*> GetInventoryItems() const { return InventoryItems; }

	
};