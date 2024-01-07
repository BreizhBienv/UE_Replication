#include "ShooterPS.h"
#include "PlayerGI.h"
#include "../Characters/ShooterCharacter.h"
#include "DeathMatchGS.h"

void AShooterPS::BeginPlay()
{
	Super::BeginPlay();

	ADeathMatchGS* GameState = GetWorld()->GetGameState<ADeathMatchGS>();

	if (GameState)
		GameState->OnResetAfterDelay.AddLambda([this]() { Reset(); });
}

void AShooterPS::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPS, Team);
	DOREPLIFETIME(AShooterPS, IsReady);
	DOREPLIFETIME(AShooterPS, NbKill);
	DOREPLIFETIME(AShooterPS, NbDeath);
}

void AShooterPS::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (PlayerState)
	{
		AShooterPS* ShooterPlayerState = Cast<AShooterPS>(PlayerState);
		if (ShooterPlayerState)
		{
			ShooterPlayerState->NbKill = NbKill;
			ShooterPlayerState->NbKill = NbDeath;
			ShooterPlayerState->Team = Team;
			ShooterPlayerState->IsReady = IsReady;
		}
	}
}

void AShooterPS::OverrideWith(class APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);
	if (PlayerState)
	{
		AShooterPS* ShooterPlayerState = Cast<AShooterPS>(PlayerState);

		if (ShooterPlayerState)
		{
			NbKill = ShooterPlayerState->NbKill;
			NbDeath = ShooterPlayerState->NbDeath;
			Team = ShooterPlayerState->Team;
			IsReady = ShooterPlayerState->IsReady;
		}
	}
}

void AShooterPS::SendNewName_Implementation(const FString& pUserName)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	SetPlayerName(pUserName);
}

void AShooterPS::SendNewTeam_Implementation(ETeam pTeam)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	Team = pTeam;
}

void AShooterPS::SendNewReady_Implementation(bool pIsReady)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	IsReady = pIsReady;
}

void AShooterPS::OnRep_Kill()
{
	OnKillChange.Broadcast(NbKill);
}

void AShooterPS::OnRep_Death()
{
	OnDeathChange.Broadcast(NbDeath);
}

void AShooterPS::OnRep_Team()
{
	OnTeamChanged.Broadcast(Team);
}

void AShooterPS::OnRep_Ready()
{
	OnReadyChanged.Broadcast(IsReady);
}

void AShooterPS::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnNameChanged.Broadcast(GetPlayerName());
}

void AShooterPS::Reset()
{
	NbKill = 0;
	NbDeath = 0;
}
