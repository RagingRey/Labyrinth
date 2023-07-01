// Labyrinth, copyright Rage Games 2023, all rights reserved.


#include "LRWeapon.h"

//#include "Characters/LRCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "LRMagazine.h"

// Sets default values
ALRWeapon::ALRWeapon()
{
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	this->RootComponent = WeaponMeshComponent;

	LineTraceComponent = CreateDefaultSubobject<ULRLineTrace>("Line Trace Component");
}

void ALRWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	SetupWeapon();
}

bool ALRWeapon::Server_SetupWeapon_Validate()
{
	return true;
}

void ALRWeapon::Server_SetupWeapon_Implementation()
{
	if(HasAuthority())
	{
		CurrentMagazine = GetWorld()->SpawnActor<ALRMagazine>(WeaponMagazine_Class, this->GetActorLocation(), this->GetActorRotation());

		if (CurrentMagazine)
		{
			CurrentMagazine->SetUpMagazine(FName(WeaponData->SupportedMagazineName));
			CurrentMagazine->PickUp();
		}

		Server_SpawnMagazines(2);
	}
}

void ALRWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALRWeapon, CurrentMagazine);
	DOREPLIFETIME(ALRWeapon, ExtraMagazines);
}

FHitResult ALRWeapon::Fire()
{
	if(CurrentMagazine && CurrentMagazine->GetMagazineCount())
	{
		FVector StartLocation = WeaponMeshComponent->GetSocketLocation("MuzzleFlash");
		FRotator Rotation = WeaponMeshComponent->GetSocketRotation("MuzzleFlash");
		FVector EndLocation = WeaponMeshComponent->GetSocketLocation("MuzzleFlash") + Rotation.Vector() * 3500;

		//CurrentMagazine->Fire();

		return LineTraceComponent->LineTraceSingle(StartLocation, EndLocation, true);
	}

	return FHitResult();
}

FHitResult ALRWeapon::Fire(FVector End)
{
	if (CurrentMagazine && CurrentMagazine->GetMagazineCount())
	{
		FVector StartLocation = WeaponMeshComponent->GetSocketLocation("MuzzleFlash");
		FVector EndLocation = StartLocation + (UKismetMathLibrary::FindLookAtRotation(StartLocation, End).Vector() * 3500);

		CurrentMagazine->Fire();

		FHitResult ClientHitResult = LineTraceComponent->LineTraceSingle(StartLocation, EndLocation, true);
		// if (ALRCharacter* Character = Cast<ALRCharacter>(ClientHitResult.GetActor()))
		// {
		// 	Character->TakeDamage(10.0f, FDamageEvent(), nullptr, this->GetOwner());
		// }

		return ClientHitResult;
	}

	return FHitResult();
}

FHitResult ALRWeapon::Fire(FHitResult HitResult)
{
	if(HasAuthority())
	{
		if (CurrentMagazine && CurrentMagazine->GetMagazineCount() > 0)
		{
			FVector StartLocation = WeaponMeshComponent->GetSocketLocation("MuzzleFlash");
			FVector EndLocation = StartLocation + (UKismetMathLibrary::FindLookAtRotation(StartLocation, HitResult.TraceEnd).Vector() * 3500);

			CurrentMagazine->Fire();

			FHitResult ServerHitResult = LineTraceComponent->LineTraceSingle(StartLocation, EndLocation, false);

			// if(ALRCharacter* Character = Cast<ALRCharacter>(ServerHitResult.GetActor()))
			// {
			// 	Character->TakeDamage(10.0f, FDamageEvent(), nullptr, this->GetOwner());
			// }
		}
	}

	return FHitResult();
}

bool ALRWeapon::CanReload()
{
	return (CurrentMagazine && CurrentMagazine->GetMagazineCount() < CurrentMagazine->GetMaxAmmoCount() && ExtraMagazines.Num() > 0);
}

void ALRWeapon::Reload()
{
	ALRMagazine* TempMagazine = CurrentMagazine;
	CurrentMagazine = ExtraMagazines[0];
	ExtraMagazines.RemoveAt(0);

	if(CurrentMagazine->GetMagazineCount() > 0)
	{
		ExtraMagazines.Add(TempMagazine);
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Ammo: %d"), CurrentMagazine->GetMagazineCount());

	for(const ALRMagazine* Magazine: ExtraMagazines)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Ammo: %d"), Magazine->GetMagazineCount())
	}
}

void ALRWeapon::SetupWeapon()
{
	if(WeaponDataTable)
	{
		TArray<FName> WeaponNames = WeaponDataTable->GetRowNames();

		const FString ContextString = WeaponNames[0].ToString();
		WeaponData = WeaponDataTable->FindRow<FLRWeaponData>(WeaponNames[0], ContextString);

		if(WeaponData)
		{
			WeaponMeshComponent->SetSkeletalMesh(WeaponData->WeaponMesh);
			FireAnimation = WeaponData->FireAnimation;
			ReloadAnimation = WeaponData->ReloadAnimation;
			WeaponFXs = WeaponData->WeaponFXs;

			if(WeaponMagazine_Class)
			{
				Server_SetupWeapon();
			}
		}
	}
}

void ALRWeapon::AddMagazine(ALRMagazine* Magazine)
{
	this->ExtraMagazines.Add(Magazine);
	UE_LOG(LogTemp, Warning, TEXT("Extra Mag: %d"), ExtraMagazines.Num());
	Magazine->PickUp();
}

bool ALRWeapon::Server_SpawnMagazines_Validate(int Amount)
{
	return true;
}

void ALRWeapon::Server_SpawnMagazines_Implementation(int Amount)
{
	if(HasAuthority())
	{
		for (size_t i{ 0 }; i < Amount; i++)
		{
			const int RandomX = FMath::RandRange(1100, 1500);
			const int RandomY = FMath::RandRange(1150, 1400);

			ALRMagazine* Magazine = GetWorld()->SpawnActor<ALRMagazine>(WeaponMagazine_Class, FVector(RandomX, RandomY, 50.0f), FRotator());
		}
	}
}