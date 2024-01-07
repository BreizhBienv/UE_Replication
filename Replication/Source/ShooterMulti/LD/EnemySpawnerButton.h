
#pragma once

#include "GameFramework/Actor.h"
#include "../Characters/UndeadCharacter.h"
#include "../GameFramework/Resetable.h"
#include "EnemySpawnerButton.generated.h"

UCLASS()
class SHOOTERMULTI_API AEnemySpawnerButton : public AActor, public IResetable
{
	GENERATED_BODY()
	
protected:

	class AUndeadDirector* UndeadDir = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam mTeam = ETeam::None;

	FTimerHandle mSpawnTimerHandle;

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	float SecondPerSpawn = 3.0f;

	UPROPERTY(EditAnywhere)
	USoundBase* ActivateSound;

	UMaterialInstanceDynamic* material;

	AEnemySpawnerButton();

	void Activate(ETeam team);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void ActivationSound();
	void ActivationSound_Implementation();
	bool ActivationSound_Validate() { return true; }

	UFUNCTION()
	void OnRep_Team();

	UFUNCTION()
	void SetTeam(ETeam team);

	UFUNCTION()
	void Reset() override;
};
