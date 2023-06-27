// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LRInteractionInterface.generated.h"

class ALRCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULRInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LABYRINTH_API ILRInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact(ALRCharacter* Character) = 0;
};
