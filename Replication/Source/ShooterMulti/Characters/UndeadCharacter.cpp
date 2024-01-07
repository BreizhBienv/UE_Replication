#include "UndeadCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "../Animations/UndeadCharacterAnim.h"
#include "../GameFramework/DeathMatchGS.h"
#include "../Controllers/UndeadAIController.h"
#include "../Weapons/DamageTypePunch.h"
#include "Components/CapsuleComponent.h"

AUndeadCharacter::AUndeadCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;
}

EUndeadCharacterState AUndeadCharacter::GetState() const
{
	return State;
}

void AUndeadCharacter::SetState(EUndeadCharacterState InState)
{
	PrevState = State;
	State = InState;

	AUndeadAIController* AIController = Cast<AUndeadAIController>(Controller);
	if (IsValid(AIController))
	{
		if (PrevState == EUndeadCharacterState::Stun)
			AIController->SetIsStun(false);
		else if (State == EUndeadCharacterState::Stun)
			AIController->SetIsStun(true);

	}

	//UE_LOG(LogTemp, Warning, TEXT("State is %s"), *UEnum::GetValueAsString(State));
}

// Called when the game starts or when spawned
void AUndeadCharacter::BeginPlay()
{
	if(Team == ETeam::None)
		SetTeam(ETeam::AI);

	Super::BeginPlay();
	
	SetState(EUndeadCharacterState::IdleRun);
}

void AUndeadCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUndeadCharacter, State);
	DOREPLIFETIME(AUndeadCharacter, PrevState);
}

void AUndeadCharacter::StartStun()
{
	if (State == EUndeadCharacterState::Stun)
		return;

	SetState(EUndeadCharacterState::Stun);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AUndeadCharacter::EndStun, StunCooldown, false);

	// Disabled since there is non proper hit animation for this character
	//PlayHitMontage();
}

void AUndeadCharacter::PlayHitMontage()
{
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetMesh());

	if (SkeletalMesh != nullptr)
	{
		UUndeadCharacterAnim* AnimInstance = Cast<UUndeadCharacterAnim>(SkeletalMesh->GetAnimInstance());
		if (AnimInstance)
			AnimInstance->PlayHitMontage();
	}
}

void AUndeadCharacter::EndStun()
{
	SetState(EUndeadCharacterState::IdleRun);
}

bool AUndeadCharacter::Punch()
{
	if (State != EUndeadCharacterState::IdleRun)
		return false;

	SetState(EUndeadCharacterState::Punch);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AUndeadCharacter::EndPunch, PunchCooldown, false);

	PlayPunchMontage();

	return true;
}

void AUndeadCharacter::EndPunch()
{
	if (State == EUndeadCharacterState::Punch)
		SetState(EUndeadCharacterState::IdleRun);
}

void AUndeadCharacter::PlayPunchMontage_Implementation()
{
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetMesh());

	if (SkeletalMesh != nullptr)
	{
		UUndeadCharacterAnim* AnimInstance = Cast<UUndeadCharacterAnim>(SkeletalMesh->GetAnimInstance());
		if (AnimInstance)
			AnimInstance->PlayPunchMontage();
	}
}

void AUndeadCharacter::StartDisapear()
{
	Super::StartDisapear();

	ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	GameState->RemoveAI();
}

void AUndeadCharacter::Reset()
{
	ActivateRagdoll();

	Super::Reset();
}

float AUndeadCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Damages = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (IsDead())
		return Damages;

	TSubclassOf<UDamageType> const DamageTypeClass = DamageEvent.DamageTypeClass;

	if (Damages > 0.0f && DamageTypeClass == UDamageTypePunch::StaticClass())
		StartStun();

	return Damages;
}

