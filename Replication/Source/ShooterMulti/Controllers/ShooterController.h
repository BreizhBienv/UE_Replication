#pragma once

#include "GameFramework/PlayerController.h"
#include "../GameFramework/DeathMatchGS.h"
#include "ShooterController.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	class AShooterCharacter* ShooterCharacter = nullptr;

	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void StartSprint();
	void EndSprint();
	void StartJump();
	void StartAim();
	void EndAim();

	UFUNCTION(Server, Reliable, WithValidation)
	void StartShoot();
	void StartShoot_Implementation();
	bool StartShoot_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void EndShoot();
	void EndShoot_Implementation();
	bool EndShoot_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void StartReload();
	void StartReload_Implementation();
	bool StartReload_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void PushButton();
	void PushButton_Implementation();
	bool PushButton_Validate() { return true; }

	void Punch();

public:
	void DisableInput(APlayerController* PlayerController) override;

	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndJump();
	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndReload();
};