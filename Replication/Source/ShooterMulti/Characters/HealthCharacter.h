#pragma once

#include "GameFramework/Character.h"
#include "../GameFramework/ShooterPS.h"
#include "../GameFramework/Resetable.h"
#include "HealthCharacter.generated.h"

UCLASS()
class SHOOTERMULTI_API AHealthCharacter : public ACharacter, public IResetable
{
	GENERATED_BODY()

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	float DisapearTimer;
	bool bIsDisapearing;
	TArray<UMaterialInstanceDynamic*> DissolveMaterials;

	UPROPERTY(BlueprintReadOnly, Category = "Character", ReplicatedUsing = OnRep_Team)
	ETeam Team;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float MaxHealth = 100.f;

	UPROPERTY(Replicated)
	float Health = MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float DisapearingDelay = 10.f;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float DisapearingDuration = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* HeadshotHitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* PunchHitSound;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character")
	class USphereComponent* PunchCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float PunchDuration = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float PunchDamage = 10.f;

	void InitRagdoll();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void ActivateRagdoll();
	void ActivateRagdoll_Implementation();
	bool ActivateRagdoll_Validate() { return true; }

public:

	DECLARE_EVENT(AHealthCharacter, TeamSwitchEvent)
	TeamSwitchEvent OnTeamSwitch;
 
	AHealthCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Character|Health")
	bool IsDead();

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GetHealth() const;

	ETeam GetTeam() const;

	void SetTeam(ETeam InTeam);

	UFUNCTION()
	void OnRep_Team();

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	virtual float	TakeDamage	(float					DamageAmount,
								 FDamageEvent const&	DamageEvent,
								 class AController*		EventInstigator,
								 AActor*				DamageCauser) override;

	UFUNCTION(Netmulticast, Reliable, WithValidation)
	void PlayHitSound(USoundBase* Sound, FVector_NetQuantize Location);
	void PlayHitSound_Implementation(USoundBase* Sound, FVector_NetQuantize Location);
	bool PlayHitSound_Validate(USoundBase* Sound, FVector_NetQuantize Location) { return true; }

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GainHealth(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	void ResetHealth();

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	void InflictPunch();

	void UpdateSkinColor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Reset() override;


	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void StartDisapear();
	virtual void StartDisapear_Implementation();
	virtual bool StartDisapear_Validate() { return true; }

	virtual void UpdateDisapear();

	virtual void FinishDisapear();
};
