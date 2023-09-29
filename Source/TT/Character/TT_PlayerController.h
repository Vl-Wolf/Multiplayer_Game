// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TT_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TT_API ATT_PlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void OnPossess(APawn* InPawn) override;

public:

	ATT_PlayerController();

protected:

	virtual void SetupInputComponent() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);

	void Jump();
	void StopJump();
	void AimPressed();
	void AimReleased();
	void SprintPressed();
	void SprintReleased();
	void FirePressed();
	void FireReleased();
	void Reload();
	void ChangeWeapon();

public:

	bool bIsFiring = false;
	bool bIsAiming = false;
	bool bIsSprinting = false;
	
};
