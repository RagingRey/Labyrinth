// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Components/LRInventory.h"

// Sets default values for this component's properties
ULRInventory::ULRInventory()
{
	
}


// Called when the game starts
void ULRInventory::BeginPlay()
{
	Super::BeginPlay();

}

void ULRInventory::AddItem(ALRPickups* Item)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		InventoryItems.Add(Item);
		Item->Picked();
	}
}