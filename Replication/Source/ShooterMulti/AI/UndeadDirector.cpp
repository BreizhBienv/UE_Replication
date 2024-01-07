#include "UndeadDirector.h"
#include "../ShooterMulti.h"
#include "../GameFramework/DeathMatchGS.h"
#include "../Characters/UndeadCharacter.h"
#include "Engine/World.h"

AUndeadDirector::AUndeadDirector()
{
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AUndeadDirector::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnPoints.Num() == 0)
		UE_LOG(GLogShooterMulti, Warning, TEXT("Undead Director has no spawn point."));

	if (GetLocalRole() == ROLE_Authority)
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUndeadDirector::SpawnTickEnemy, SecondPerSpawn, true);
	
	//ADeathMatchGS* temp = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	//temp->OnPlayerNum.AddLambda([this](ADeathMatchGS* gs) { UpdateFrequencies(gs); });
}

void AUndeadDirector::SpawnEnemy(FVector pos, const FRotator& rot, ETeam Team)
{
	ADeathMatchGS* temp = Cast<ADeathMatchGS>(GetWorld()->GetGameState());

	ENetRole role = GetLocalRole();

	if (SpawnPoints.Num() == 0 || !temp->CanAddAI())
		return;

	pos.Y += 100; // avoid in ground spawn.

	temp->AddAI();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	auto undead = GetWorld()->SpawnActor<AUndeadCharacter>(UndeadBlueprint, pos, rot, spawnParameters);

	undead->SetTeam(Team);
}

void AUndeadDirector::SpawnTickEnemy()
{
	int rand = FMath::RandRange(0, SpawnPoints.Num() - 1);

	SpawnEnemy(SpawnPoints[rand]->GetActorLocation(), SpawnPoints[rand]->GetActorRotation());
}