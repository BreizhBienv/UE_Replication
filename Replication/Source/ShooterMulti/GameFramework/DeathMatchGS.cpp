#include "DeathMatchGS.h"
#include "ShooterPS.h"
#include "TimerManager.h"
#include "DeathMatchGM.h"
#include "../Characters/ShooterCharacter.h"
#include "../LD/Pickup.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

ADeathMatchGS::ADeathMatchGS()
{
	bReplicates = true;
}

void ADeathMatchGS::BeginPlay()
{
	Super::BeginPlay();

	OnTeamWin.AddLambda([this](ETeam Team) { ShowTeamWinHUD(Team); });

	OnGameRestart.AddLambda([this]() { Reset(); });

	GameMode = Cast<ADeathMatchGM>(AuthorityGameMode);

	if (GetLocalRole() == ROLE_Authority)
	{
		check(GameMode && "GameMode nullptr: Cast as ADeathMatchGM failed.");

		CurrentTime = GameMode->GameTime;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);
	}
}

void ADeathMatchGS::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathMatchGS, CurrentTime);
	DOREPLIFETIME(ADeathMatchGS, BlueTeamScore);
	DOREPLIFETIME(ADeathMatchGS, RedTeamScore);
}

void ADeathMatchGS::AdvanceTimer()
{
	--CurrentTime;
	
	if (CurrentTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (RedTeamScore < BlueTeamScore)
			UpdateEndHud(ETeam::Blue);
		else if (RedTeamScore > BlueTeamScore)
			UpdateEndHud(ETeam::Red);
		else
			UpdateEndHud(ETeam::None);
	}
}

void ADeathMatchGS::AddScore(ETeam Team)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (Team == ETeam::Red && ++RedTeamScore == GameMode->MaxKill)
		UpdateEndHud(ETeam::Red);
	else if (Team == ETeam::Blue && ++BlueTeamScore == GameMode->MaxKill)
		UpdateEndHud(ETeam::Blue);
}

void ADeathMatchGS::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	//AShooterPS* ps = Cast<AShooterPS>(PlayerState);
	//if (ps && ps->Team != ETeam::None)
	//{
	//	ETeam team = PlayerArray.Num() % 2 == 0 ? ETeam::Red : ETeam::Blue;
	//	ps->Team = team;
	//}

	OnPlayerNum.Broadcast(this);
}

void ADeathMatchGS::RemovePlayerState(APlayerState* PlayerState)
{
	OnPlayerNum.Broadcast(this);

	Super::RemovePlayerState(PlayerState);
}

bool ADeathMatchGS::CanAddAI()
{
	return Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->MaxAIPerPlayer* PlayerArray.Num() > CurrentAICount;
}

void ADeathMatchGS::AddAI()
{
	CurrentAICount++;
}

void ADeathMatchGS::RemoveAI()
{
	CurrentAICount--;
}

int ADeathMatchGS::GetNbplayer()
{
	return PlayerArray.Num();
}

void ADeathMatchGS::UpdateEndHud_Implementation(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();
}

void ADeathMatchGS::ResetAfterDelay()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentTime = GameMode->GameTime;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

		RedTeamScore = 0;
		BlueTeamScore = 0;
		CurrentAICount = 0;

		OnResetAfterDelay.Broadcast();
	}
}	

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}
