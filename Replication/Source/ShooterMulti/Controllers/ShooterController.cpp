#include "ShooterController.h"
#include "../Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (GetLocalRole() == ROLE_Authority)
		ShooterCharacter->InitPlayer();
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AShooterController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterController::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AShooterController::LookUp);
	InputComponent->BindAxis("Turn", this, &AShooterController::Turn);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooterController::EndSprint);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AShooterController::StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &AShooterController::EndAim);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterController::StartReload);
	InputComponent->BindAction("Punch", IE_Pressed, this, &AShooterController::Punch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterController::StartJump);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterController::StartShoot);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooterController::EndShoot);
	InputComponent->BindAction("PushButton", IE_Pressed, this, &AShooterController::PushButton);

	ENetRole role = GetLocalRole();
}

void AShooterController::MoveForward(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		if (ShooterCharacter->GetState() == EShooterCharacterState::Sprint && Value <= 0.0f)
			EndSprint();

		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().Vector());
	}

}
void AShooterController::MoveRight(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead()	&& ShooterCharacter->GetState() != EShooterCharacterState::Sprint
																	&& ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().RotateVector(FVector::RightVector));
	}
}

void AShooterController::LookUp(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddPitchInput(Value);
	}
}
void AShooterController::Turn(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddYawInput(Value);
	}
}

void AShooterController::StartSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartSprint();
}

void AShooterController::EndSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndSprint();
}

void AShooterController::StartJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartJump();
}

void AShooterController::EndJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndJump();
}


void AShooterController::StartAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
	{
		ShooterCharacter->SetState(EShooterCharacterState::Aim);

		ShooterCharacter->StartAimCam();
		ShooterCharacter->StartAim();
	}
}

void AShooterController::EndAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
	{
		ShooterCharacter->EndAimCam();
		ShooterCharacter->EndAim();
	}
}

//void AShooterController::StartShoot()
//{
//	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
//		ShooterCharacter->StartShoot();
//}

//void AShooterController::EndShoot()
//{
//	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
//		ShooterCharacter->EndShoot();
//}

void AShooterController::StartShoot_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartShoot();
}

void AShooterController::EndShoot_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndShoot();
}

//void AShooterController::StartReload()
//{
//	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
//		ShooterCharacter->StartReload();
//}


void AShooterController::StartReload_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartReload();
}

void AShooterController::EndReload()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && GetLocalRole() == ROLE_Authority)
		ShooterCharacter->EndReload();
}

void AShooterController::PushButton_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && GetLocalRole() == ROLE_Authority)
		ShooterCharacter->PushButton();
}

void AShooterController::Punch()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->Punch();
}

void AShooterController::DisableInput(APlayerController* PlayerController)
{
	Super::DisableInput(PlayerController);
	
	EndSprint();
	EndAim();
	EndShoot();
}