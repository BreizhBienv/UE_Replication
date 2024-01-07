#pragma once

#include "Pickup.h"
#include "../GameFramework/Resetable.h"
#include "GameFramework/Actor.h"
#include "PickupDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API APickupDirector : public AActor, public IResetable
{
	GENERATED_BODY()
	
private:
	TArray<bool> IsSpawnFullArray;
	int CurrentPickupIndex = 0;

	FTimerHandle TimerHandle;

	bool bIsFull = false;

protected:

	virtual void BeginPlay() override;

public:

	APickupDirector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 1.0f;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<TSubclassOf<APickup>> PickupBPs;
	
	void SpawnTick();

	void FreePickup(FSpawnKey Key);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SpawnPickup(int pickupIndex, int spawnPointIndex);
	void SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex);
	bool SpawnPickup_Validate(int pickupIndex, int spawnPointIndex) { return true; }

	void SetFull(bool isFull);

	//void UpdateFrequencies(class ADeathMatchGS* GameState);

	virtual void Reset() override;
};