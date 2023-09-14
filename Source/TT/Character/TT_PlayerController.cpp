// Fill out your copyright notice in the Description page of Project Settings.


#include "TT/Character/TT_PlayerController.h"

#include "GameFramework/Character.h"

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
