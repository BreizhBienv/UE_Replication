#pragma once

#include "GameFramework/PlayerState.h"
#include "DeathMatchGM.h"
#include "PlayerGI.h"
#include "ShooterPS.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamChanged, ETeam, NewTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReadyChanged, bool, NewReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNameChanged, FString, NewName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKillChange, int, NewKillNb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathChange, int, NewDeathNb);

UCLASS()
class SHOOTERMULTI_API AShooterPS : public APlayerState
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(BlueprintAssignable)
	FKillChange OnKillChange;
	UPROPERTY(BlueprintAssignable)
	FDeathChange OnDeathChange;

	UPROPERTY(BlueprintAssignable)
	FTeamChanged OnTeamChanged;
	UPROPERTY(BlueprintAssignable)
	FReadyChanged OnReadyChanged;
	UPROPERTY(BlueprintAssignable)
	FNameChanged OnNameChanged;


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Kill)
	int NbKill;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Death)
	int NbDeath;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_Team)
	ETeam Team = ETeam::Blue;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_Ready)
	bool IsReady = false;

	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SendNewName(const FString& pUserName);
	void SendNewName_Implementation(const FString& pUserName);
	bool SendNewName_Validate(const FString& pUserName) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SendNewTeam(ETeam pTeam);
	void SendNewTeam_Implementation(ETeam pTeam);
	bool SendNewTeam_Validate(ETeam pTeam) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SendNewReady(bool pIsReady);
	void SendNewReady_Implementation(bool pIsReady);
	bool SendNewReady_Validate(bool pIsReady) { return true; }
	
	UFUNCTION()
	void OnRep_Kill();

	UFUNCTION()
	void OnRep_Death();

	UFUNCTION()
	void OnRep_Team();

	UFUNCTION()
	void OnRep_Ready();

	virtual void OnRep_PlayerName() override;

	UFUNCTION()
	void Reset();
};
