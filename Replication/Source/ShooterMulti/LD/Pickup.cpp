#include "Pickup.h"
#include "PickupDirector.h"
#include "../Characters/ShooterCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	BasePos = GetActorLocation();
}

void APickup::Tick(float DeltaTime)
{
	GlobalTime += DeltaTime;
	float Offset = 10 * sin(GlobalTime);

	SetActorLocation(BasePos + FVector::UpVector * Offset);

	FRotator NewRot = GetActorRotation();
	NewRot.Yaw += 42 * sin(DeltaTime);
	SetActorRotation(NewRot);
}

void APickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);

	if (!IsValid(Player))
		return;

	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());

	if (IsValid(Director))
		Director->FreePickup(SpawnKey);
	
	Destroy();
}

void APickup::Reset()
{
	Destroy();
}
