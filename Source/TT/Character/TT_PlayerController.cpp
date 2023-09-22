// Fill out your copyright notice in the Description page of Project Settings.


#include "TT/Character/TT_PlayerController.h"

#include "TTCharacter.h"
#include "GameFramework/Character.h"
#include "TT/Game/TT_PlayerState.h"

void ATT_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ATTCharacter* MyCharacter = Cast<ATTCharacter>(InPawn);
	if (MyCharacter)
	{
		ATT_PlayerState* MyPlayerState = GetPlayerState<ATT_PlayerState>();
		{
			MyCharacter->GetInventoryComponent()->InitInventory_OnServer(MyPlayerState->WeaponSlot, MyPlayerState->AmmoSlot);
		}
	}
}

ATT_PlayerController::ATT_PlayerController()
{
	this->bReplicates = true;
}

void ATT_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Axis
	InputComponent->BindAxis("MoveForward", this, &ATT_PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATT_PlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ATT_PlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ATT_PlayerController::LookUp);

	//Action
	InputComponent->BindAction("Jump", IE_Pressed, this, &ATT_PlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ATT_PlayerController::StopJump);
	InputComponent->BindAction("Aim", IE_Pressed, this, &ATT_PlayerController::Aim);
	InputComponent->BindAction("Aim", IE_Released, this, &ATT_PlayerController::Aim);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ATT_PlayerController::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATT_PlayerController::Sprint);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ATT_PlayerController::Reload);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATT_PlayerController::Fire);
	InputComponent->BindAction("Fire", IE_Released, this, &ATT_PlayerController::Fire);
	InputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &ATT_PlayerController::ChangeWeapon);
	
}

void ATT_PlayerController::MoveForward(float value)
{
	if (value != 0.0f && GetPawn())
	{
		APawn* MyPawn = GetPawn();
		const FRotator PawnControlRotation = GetControlRotation();
		const FVector ForwardVector = FRotationMatrix(PawnControlRotation).GetScaledAxis(EAxis::X);
		MyPawn->AddMovementInput(ForwardVector, value);
	}
}

void ATT_PlayerController::MoveRight(float value)
{
	if (value != 0.0f && GetPawn())
	{
		APawn* MyPawn = GetPawn();
		const FRotator PawnControlRotation = GetControlRotation();
		const FVector RightVector = FRotationMatrix(PawnControlRotation).GetScaledAxis(EAxis::Y);
		MyPawn->AddMovementInput(RightVector, value);
	}
}

void ATT_PlayerController::Turn(float value)
{
	AddYawInput(value);
}

void ATT_PlayerController::LookUp(float value)
{
	AddPitchInput(value);
}

void ATT_PlayerController::Jump()
{
	if (GetPawn())
	{
		ACharacter* MyCharacter = Cast<ACharacter>(GetPawn());
		if (MyCharacter)
		{
			MyCharacter->Jump();
		}
	}
}

void ATT_PlayerController::StopJump()
{
	if (GetPawn())
	{
		ACharacter* MyCharacter = Cast<ACharacter>(GetPawn());
		if (MyCharacter)
		{
			MyCharacter->StopJumping();
		}
	}
}

void ATT_PlayerController::Aim()
{
	ATTCharacter* MyCharacter = Cast<ATTCharacter>(GetPawn());
	
	if (!bIsAiming)
	{
		if (MyCharacter)
		{
			bIsAiming = true;
			MyCharacter->InputAimPressed();
		}
	}
	else
	{
		if (MyCharacter)
		{
			bIsAiming = false;
			MyCharacter->InputAimReleased();
		}
	}
}

void ATT_PlayerController::Sprint()
{
	ATTCharacter* MyCharacter = Cast<ATTCharacter>(GetPawn());
	
	if (!bIsSprinting)
	{
		if (MyCharacter)
		{
			bIsSprinting = true;
			MyCharacter->InputSprintPressed();
		}
	}
	else
	{
		if (MyCharacter)
		{
			bIsSprinting = false;
			MyCharacter->InputSprintReleased();
		}
	}
}

void ATT_PlayerController::Reload()
{
	ATTCharacter* MyCharacter = Cast<ATTCharacter>(GetPawn());
	
	if (MyCharacter)
	{
		MyCharacter->TryReloadWeapon();
	}
}

void ATT_PlayerController::ChangeWeapon()
{
	ATTCharacter* MyCharacter = Cast<ATTCharacter>(GetPawn());
	
	if (MyCharacter)
	{
		MyCharacter->TrySwitchWeapon();
	}
}

void ATT_PlayerController::Fire()
{
	ATTCharacter* MyCharacter = Cast<ATTCharacter>(GetPawn());
	
	if (!bIsFiring)
	{
		if (MyCharacter)
		{
			bIsFiring = true;
			MyCharacter->AttackEvent(bIsFiring);
		}
	}
	else
	{
		if (MyCharacter)
		{
			bIsFiring = false;
			MyCharacter->AttackEvent(bIsFiring);
		}
	}
}
