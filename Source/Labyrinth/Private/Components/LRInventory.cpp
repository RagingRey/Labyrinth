// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "Components/LRInventory.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
ULRInventory::ULRInventory()
{
	
}


// Called when the game starts
void ULRInventory::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void ULRInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULRInventory, InventoryItems);
}

void ULRInventory::AddItem(ALRPickups* Item)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		InventoryItems.Add(Item);
		Item->Owner = this->GetOwner();
		Item->Picked();
	}
}

void ULRInventory::UseItem(ALRPickups* Item)
{
	//if(GetOwnerRole() == ROLE_Authority)
	{
		Item->UseItem();
		InventoryItems.Remove(Item);
	}
}
