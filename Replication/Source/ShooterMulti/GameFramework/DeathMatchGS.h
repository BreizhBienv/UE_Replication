#pragma once

#include "DeathMatchGM.h"
#include "GameFramework/GameStateBase.h"
#include "DeathMatchGS.generated.h"

class AHealthCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKillFeedback, AHealthCharacter*, Killer, AHealthCharacter*, Killed);

UCLASS()
class SHOOTERMULTI_API ADeathMatchGS : public AGameStateBase
{
	GENERATED_BODY()

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FTimerHandle CountdownTimerHandle;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Shooter|GameState")
	ADeathMatchGM* GameMode = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState", Replicated)
	int32 CurrentTime;

	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState")
	int32 CurrentAICount = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState", Replicated)
	int32 RedTeamScore = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState", Replicated)
	int32 BlueTeamScore = 0;

	void AdvanceTimer();

	UFUNCTION()
	void Reset();

public:

	DECLARE_EVENT_OneParam(ADeathMatchGS, FOnPlayerAddAndRemove, ADeathMatchGS*)
	FOnPlayerAddAndRemove OnPlayerNum;
	DECLARE_EVENT_OneParam(ADeathMatchGS, TeamWinEvent, ETeam)
	TeamWinEvent OnTeamWin;
	DECLARE_EVENT(ADeathMatchGS, GameRestartEvent)
	GameRestartEvent OnGameRestart;
	GameRestartEvent OnResetAfterDelay;

	UPROPERTY(BlueprintAssignable)
	FKillFeedback OnKillFeedback;

	ADeathMatchGS();

	void BeginPlay() override;

	void AddScore(ETeam Team);
	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	bool CanAddAI();
	void AddAI();
	void RemoveAI();

	int GetNbplayer();

	UFUNCTION(NetMulticast, Reliable)
	void UpdateEndHud(ETeam Team);

	UFUNCTION(BlueprintCallable)
	void EndGameTrigg();

	UFUNCTION(BlueprintCallable)
	void ResetAfterDelay();

	UFUNCTION(BlueprintNativeEvent)
	void ShowTeamWinHUD(ETeam Team);
	void ShowTeamWinHUD_Implementation(ETeam Team) {};
	
	int NewFrequency(int Sec);
};
