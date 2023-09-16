// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "TT/FuncLibrary/Types.h"
#include "TT_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TT_API UTT_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSetting")
	UDataTable* WeaponInfoTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSetting")
	UDataTable* DropItemInfoTable = nullptr;

	/*UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo &OutInfo);

	UFUNCTION(BlueprintCallable)
	bool GetDropItemInfoByWeaponName(FName NameItem, FDropItem &OutInfo);

	UFUNCTION(BlueprintCallable)
	bool GetDropItemInfoByName(FName NameItem, FDropItem &OutInfo);*/
	
};
