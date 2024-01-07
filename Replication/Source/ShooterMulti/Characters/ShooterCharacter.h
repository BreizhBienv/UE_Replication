#pragma once

#include "HealthCharacter.h"
#include "../Weapons/WeaponComponent.h"
#include "PlayerCameraComponent.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EShooterCharacterState : uint8
{
	IdleRun,
	Aim,
	Sprint,
	Reload,
	Jump,
	Falling,
	Punch,
	Dead,
	PushButton
};

UCLASS()
class SHOOTERMULTI_API AShooterCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UWeaponComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Shooter", Replicated)
	EShooterCharacterState State;

	UPROPERTY(Replicated)
	EShooterCharacterState PrevState;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float AimYaw;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void UpdateAimOffsets(float Pitch, float Yaw);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void PlayPushButtonAnim();
	void PlayPushButtonAnim_Implementation();
	bool PlayPushButtonAnim_Validate() { return true; }

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayPunchAnim();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_NotifyPunch();
	bool Server_NotifyPunch_Validate() { return true; }

	void Falling() override;

	void BeginPlay() override;

	void Invincibility(float Duration);

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void InvincibilityFX(float Duration);
	void InvincibilityFX_Implementation(float Duration) {};

public:

	bool bIsShooting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimArmLength = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float StandardArmLength = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimFOV = 75.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float StandardFOV = 90.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float SprintSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimWalkSpeed = 180.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float ReloadWalkSpeed = 200.f;

	UPROPERTY(BlueprintReadOnly)
	float RunSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MinSprintMagnitude = .3f;

	AShooterCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	EShooterCharacterState GetState() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void SetState(EShooterCharacterState InState);

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UWeaponComponent* GetWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UCameraComponent* GetCameraComponent();

	void InitPlayer();

	void InitTeamColor(ETeam InTeam);

	void Tick(float DeltaTime) override;

	UFUNCTION(Client, Reliable, WithValidation)
	void SetClientSpeed(float newSpeed);
	bool SetClientSpeed_Validate(float newSpeed) { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void StartSprint();
	bool StartSprint_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void EndSprint();
	bool EndSprint_Validate() { return true; }

	UFUNCTION()
	void StartJump();
	UFUNCTION()
	void EndJump();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter", Server, Reliable, WithValidation)
	void StartAim();
	bool StartAim_Validate() { return true; }

	UFUNCTION(Client, Reliable, WithValidation)
	void StartAimCam();
	bool StartAimCam_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter", Server, Reliable, WithValidation)
	void EndAim();
	bool EndAim_Validate() { return true; }

	void EndAimCamLocal();

	UFUNCTION(Client, Reliable, WithValidation)
	void EndAimCam();
	bool EndAimCam_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartShoot();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndShoot();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void AbortReload();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void PushButton();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void InflictPushButton();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void Punch();

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void RefreshTeamHUD(ETeam InTeam);
	void RefreshTeamHUD_Implementation(ETeam InTeam) {};

	void StartDisapear() override;
	void FinishDisapear() override;
};