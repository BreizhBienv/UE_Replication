#include "ShooterCharacter.h"
#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimBlueprint.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

AShooterCharacter::AShooterCharacter()
{
	DisapearingDelay = 1.5f;

	bReplicates = true;

	// Animation is set in ShooterCharacter_BP to fix build.
	//// Set Animations
	//ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimContainer(TEXT("AnimBlueprint'/Game/Blueprints/Animations/ShooterAnim_BP.ShooterAnim_BP'"));

	//if (AnimContainer.Succeeded())
	//	GetMesh()->SetAnimInstanceClass(AnimContainer.Object->GeneratedClass);

	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	// Create Sprint Arm and Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->ProbeSize = 12.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeLocation(FVector(30.f, 0.f, 100.f));

	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	PrevState = State;
	State = InState;
}

UWeaponComponent* AShooterCharacter::GetWeaponComponent()
{
	return Weapon;
}

UCameraComponent* AShooterCharacter::GetCameraComponent()
{
	return Camera;
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, State);
	DOREPLIFETIME(AShooterCharacter, PrevState);
	DOREPLIFETIME(AShooterCharacter, AimPitch);
	DOREPLIFETIME(AShooterCharacter, AimYaw);
}

void AShooterCharacter::UpdateAimOffsets_Implementation(float Pitch, float Yaw)
{
	AimPitch = Pitch;
	AimYaw = Yaw;
}

void AShooterCharacter::InitPlayer()
{
	//const FPlayerInfo& PlayerInfo = static_cast<UPlayerGI*>(GetGameInstance())->GetUserInfo();
	//InitTeamColor(static_cast<ETeam>(PlayerInfo.TeamNum));
	
	AShooterPS* ps = Cast<AShooterPS>(GetPlayerState());
	if (!ps)
		return;

	InitTeamColor(ps->Team);
}

void AShooterCharacter::InitTeamColor(ETeam InTeam)
{
	SetTeam(InTeam);
	OnTeamSwitch.Broadcast();
}

void AShooterCharacter::Invincibility(float Duration)
{
	Health = 100000;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() { Health = MaxHealth; }, Duration, false);

	InvincibilityFX(Duration);
}

void AShooterCharacter::BeginPlay()
{
	OnTeamSwitch.AddLambda([this]() { RefreshTeamHUD(Team); });
	
	Super::BeginPlay();

	RunSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (GetLocalRole() == ROLE_Authority)
		Invincibility(Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->InvincibilityTime);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;

	//if (bIsShooting && !Weapon->Shot())
	//	StartReload();

	// Anim aim offsets
	FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	float newPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
	float newYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f);

	UpdateAimOffsets(newPitch, newYaw);

	//Camera->ShakeCamera(uint8(State), GetLastMovementInputVector().Size());
}

void AShooterCharacter::StartAimCam_Implementation()
{
	SpringArm->TargetArmLength = AimArmLength;
	Camera->FieldOfView = AimFOV;
}

void AShooterCharacter::SetClientSpeed_Implementation(float newSpeed)
{
	if (GetLocalRole() != ROLE_AutonomousProxy)
		return;

	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

void AShooterCharacter::StartSprint_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::Sprint;
	else
		SetState(EShooterCharacterState::Sprint);

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	SetClientSpeed(SprintSpeed);
}

void AShooterCharacter::EndSprint_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (State != EShooterCharacterState::Sprint && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::IdleRun;
	else
		SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	SetClientSpeed(RunSpeed);
}

void AShooterCharacter::StartJump()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (CanJump() && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Sprint))
	{
		SetState(EShooterCharacterState::Jump);
		Jump();
	}
}

void AShooterCharacter::EndJump()
{
	if (State != EShooterCharacterState::Jump && State != EShooterCharacterState::Falling)
		return;

	SetState(EShooterCharacterState::IdleRun);
	StopJumping();
}

void AShooterCharacter::StartAim_Implementation()
{
	if (GetLocalRole() != ROLE_Authority) 
		return;

	if (State != EShooterCharacterState::IdleRun)
		return;
	
	SetState(EShooterCharacterState::Aim);

	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;

	if (GetLocalRole() == ROLE_Authority)
		SetClientSpeed(AimWalkSpeed);
}

void AShooterCharacter::EndAim_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (State != EShooterCharacterState::Aim)
		return;

	SetState(PrevState);
	
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	
	if (GetLocalRole() == ROLE_Authority)
		SetClientSpeed(RunSpeed);
	
	EndAimCam();
}

void AShooterCharacter::EndAimCam_Implementation()
{
	SpringArm->TargetArmLength = StandardArmLength;
	Camera->FieldOfView = StandardFOV;
}

void AShooterCharacter::StartShoot()
{
	if (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Aim)
	{
		if (!Weapon->CanShot())
			return;

		bIsShooting = true;
		Weapon->Shot();
	}
}

void AShooterCharacter::EndShoot()
{
	bIsShooting = false;
}

void AShooterCharacter::StartReload()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;

		SetState(EShooterCharacterState::Reload);
		
		GetCharacterMovement()->MaxWalkSpeed = ReloadWalkSpeed;
		
		if (GetLocalRole() == ROLE_Authority)
			SetClientSpeed(ReloadWalkSpeed);
	}
}

void AShooterCharacter::EndReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);
	
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	if (GetLocalRole() == ROLE_Authority)
		SetClientSpeed(RunSpeed);

	if(Weapon)
		Weapon->Reload();
}
void AShooterCharacter::AbortReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::Falling()
{
	Super::Falling();

	if (State == EShooterCharacterState::Jump)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::PushButton()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);
	PlayPushButtonAnim();
}

void AShooterCharacter::InflictPushButton()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0 && GetLocalRole() == ROLE_Authority)
	{
		AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(OverlappingActors[0]);

		if (Button && Button->IsValidLowLevel())
			Button->Activate(Team);

	}

	SetState(EShooterCharacterState::IdleRun);
}

void AShooterCharacter::PlayPushButtonAnim_Implementation()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::Punch()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);

	if (GetLocalRole() == ROLE_AutonomousProxy)
		Server_NotifyPunch();
}

void AShooterCharacter::Multi_PlayPunchAnim_Implementation()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::Server_NotifyPunch_Implementation()
{
	Multi_PlayPunchAnim();
}

void AShooterCharacter::StartDisapear()
{
	Super::StartDisapear();
	
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [this]() { Weapon->SetVisibility(false, true); }, 3.5f, false);

	if (Controller)
	{
		APlayerController* PlayerControler = Cast<APlayerController>(Controller);
		PlayerControler->DisableInput(PlayerControler);
		
		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, [PlayerControler]() { PlayerControler->EnableInput(PlayerControler); }, 5.0f, false);
	}
}

void AShooterCharacter::FinishDisapear()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	Super::FinishDisapear();

	if (GetLocalRole() == ROLE_Authority)
		Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->Respawn(PlayerController);
}
