#include "PickupDirector.h"
#include "Engine/World.h"
#include "../GameFramework/DeathMatchGS.h"
#include <Net/UnrealNetwork.h>

APickupDirector::APickupDirector()
{
	bReplicates = true;

	/* --- Needed to make SpawnPickup work --- */
	bAlwaysRelevant = true;
}

void APickupDirector::BeginPlay()
{
	Super::BeginPlay();

	IsSpawnFullArray.SetNum(SpawnPoints.Num());
	
	if (GetLocalRole() == ROLE_Authority)
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickupDirector::SpawnTick, SecondPerSpawn, true);

	//ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	//GameState->OnPlayerNum.AddLambda([this](ADeathMatchGS* GS) { UpdateFrequencies(GS); }); // ??
}

void APickupDirector::SpawnTick()
{
	if (bIsFull)
		return;
	
	int MaxPoints = SpawnPoints.Num() - 1;
	int RandomPoint = FMath::RandRange(0, MaxPoints);
	int PrevPoint = RandomPoint;

	while (IsSpawnFullArray[RandomPoint])
	{
		RandomPoint = (RandomPoint + 1) % MaxPoints;
		if (RandomPoint == PrevPoint)
		{
			bIsFull = true;
			return;
		}
	}

	IsSpawnFullArray[RandomPoint] = true;
	SpawnPickup(CurrentPickupIndex, RandomPoint);
	CurrentPickupIndex = (CurrentPickupIndex + 1) % PickupBPs.Num();
}

void APickupDirector::SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex)
{
	auto pickupBP = PickupBPs[pickupIndex];
	auto pickupLocation = SpawnPoints[spawnPointIndex]->GetActorLocation();
	auto pickupRotation = SpawnPoints[spawnPointIndex]->GetActorRotation();

	auto Pickup = GetWorld()->SpawnActor<APickup>(pickupBP, pickupLocation, pickupRotation);

	if (GetLocalRole() == ROLE_Authority && Pickup)
	{
		Pickup->SpawnKey.ClassKey = pickupIndex;
		Pickup->SpawnKey.SpawnPointKey = spawnPointIndex;
		Pickup->Director = this;
	}
}

void APickupDirector::FreePickup(FSpawnKey Key)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		IsSpawnFullArray[Key.SpawnPointKey] = false;
		bIsFull = false;
	}
}

void APickupDirector::SetFull(bool isFull)
{
	bIsFull = isFull;
}

void APickupDirector::Reset()
{
	bIsFull = false;

	for (int i = 0; i < IsSpawnFullArray.Num(); i++)
		IsSpawnFullArray[i] = false;
}
