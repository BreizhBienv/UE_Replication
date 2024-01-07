#pragma once

#include "GameFramework/Actor.h"
#include "../LD/EnemySpawnerButton.h"
#include "../Characters/UndeadCharacter.h"
#include "UndeadDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API AUndeadDirector : public AActor
{
	GENERATED_BODY()
	
protected:

	FTimerHandle SpawnTimerHandle;

	FDelegateHandle PunchEventHandle;

public:
	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TSubclassOf<class AUndeadCharacter> UndeadBlueprint;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AEnemySpawnerButton*> ButtonSpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	int MaxPunchPerSecond = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta=(ClampMin = 0.1f))
	float SecondPerSpawn = 5.0f;

	AUndeadDirector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemy(FVector pos, const FRotator& rot, ETeam Team = ETeam::AI);
	void SpawnTickEnemy();
};
