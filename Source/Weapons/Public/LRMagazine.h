// Labyrinth, copyright Rage Games 2023, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LRInteractionInterface.h"
#include "LRMagazine.generated.h"

USTRUCT()
struct FLRMagazineData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		class UStaticMesh* MagazineStaticMesh;

	UPROPERTY(EditAnywhere)
		FString MagazineName;

	UPROPERTY(EditAnywhere)
		TArray<FString> CompatibleWeapons;

	UPROPERTY(EditAnywhere)
		int MagazineCapacity;
};

UCLASS()
class WEAPONS_API ALRMagazine : public AActor
{
	GENERATED_BODY()

public:
	ALRMagazine();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		TObjectPtr<UStaticMeshComponent> MagazineMeshComponent;

	UPROPERTY(Replicated)
		int MagazineAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_MagazineUsed)
		bool bMagazineInUse;

	UPROPERTY(EditAnywhere)
		UDataTable* MagazineDataTable;

	FLRMagazineData* MagazineData;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnRep_MagazineUsed();

public:
	void SetUpMagazine(FName MagazineName);

	int GetMagazineCount() const { return MagazineAmmo; }
	int GetMaxAmmoCount() const { return MagazineData ? MagazineData->MagazineCapacity : 0; }
	void Fire() { --MagazineAmmo; UE_LOG(LogTemp, Warning, TEXT("%d"), MagazineAmmo); }
	void PickUp();

	//virtual void Interact(ALRCharacter* Character) override;
};
