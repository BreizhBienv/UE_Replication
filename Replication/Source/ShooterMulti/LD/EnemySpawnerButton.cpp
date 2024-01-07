#include "EnemySpawnerButton.h"
#include "../AI/UndeadDirector.h"
#include "../GameFramework/DeathMatchGS.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawnerButton::AEnemySpawnerButton()
{
	bReplicates = true;
}

void AEnemySpawnerButton::BeginPlay()
{
	Super::BeginPlay();

	if (material == nullptr)
	{
		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		material = Components[0]->CreateAndSetMaterialInstanceDynamic(2);
	}

	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AUndeadDirector::StaticClass());
	UndeadDir = Cast<AUndeadDirector>(actor);
}

void AEnemySpawnerButton::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemySpawnerButton, mTeam);
}

void AEnemySpawnerButton::Activate(ETeam team)
{
	if (mTeam == team)
		return;

	SetTeam(team);

	auto lambda = [this]()
	{
		if (UndeadDir)
			UndeadDir->SpawnEnemy(GetActorLocation(), GetActorRotation(), mTeam);
	};

	GetWorld()->GetTimerManager().SetTimer(mSpawnTimerHandle, lambda, SecondPerSpawn, true);

	ActivationSound();
}

void AEnemySpawnerButton::ActivationSound_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSound, GetActorLocation());
}

void AEnemySpawnerButton::OnRep_Team()
{
	SetTeam(mTeam);
}

void AEnemySpawnerButton::SetTeam(ETeam team)
{
	mTeam = team;

	if (!material)
		return;

	FLinearColor color = mTeam == ETeam::Blue ? FLinearColor::Blue : mTeam == ETeam::Red ? FLinearColor::Red : FLinearColor::Green;

	material->SetVectorParameterValue("ColorActive", color);
}

void AEnemySpawnerButton::Reset()
{
	SetTeam(ETeam::None);

	GetWorldTimerManager().ClearTimer(mSpawnTimerHandle);
}
