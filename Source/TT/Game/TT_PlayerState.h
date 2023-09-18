// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TT/FuncLibrary/Types.h"
#include "TT_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TT_API ATT_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ATT_PlayerState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FWeaponSlot> WeaponSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FAmmoSlot> AmmoSlot;
};
