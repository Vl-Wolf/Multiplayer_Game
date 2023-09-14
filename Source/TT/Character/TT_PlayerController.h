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
	
};
