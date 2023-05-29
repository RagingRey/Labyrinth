// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LRPlayerStats.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LABYRINTH_API ULRPlayerStats : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRPlayerStats();

protected:
	UPROPERTY(Replicated)
		float Health;

	UPROPERTY(Replicated)
		float Armor;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_LowerHealth(float Damage);
	bool Server_LowerHealth_Validate(float Damage);
	void Server_LowerHealth_Implementation(float Damage);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_LowerArmor(float Damage);
	bool Server_LowerArmor_Validate(float Damage);
	void Server_LowerArmor_Implementation(float Damage);

public:
	//Damage
	void LowerHealth(float Damage);
	void LowerArmor(float Damage);

	//Getters
	float GetHealth() const { return Health; }
	float GetArmor() const { return Armor; }
};
