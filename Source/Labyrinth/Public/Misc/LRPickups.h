// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LRPickups.generated.h"

UENUM()
enum class EPickupType : uint8
{
	Default,
	Grenade,
	FirstAid
};

UCLASS()
class LABYRINTH_API ALRPickups : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALRPickups();

protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
		TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
		EPickupType PickupType;
	
	UPROPERTY(ReplicatedUsing = OnRep_Picked)
		bool bPicked;

	UFUNCTION()
		void OnRep_Picked();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		UTexture2D* GetIcon() const { return Icon; }
	
	FORCEINLINE EPickupType GetPickupType() const { return PickupType; }

	UFUNCTION(BlueprintCallable)
	void UseItem();
	
	void Picked();
};
