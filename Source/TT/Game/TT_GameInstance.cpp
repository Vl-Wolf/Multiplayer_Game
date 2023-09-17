// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_GameInstance.h"

bool UTT_GameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;

	if (WeaponInfoTable)
	{
		FWeaponInfo* WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTT_GameInstance::GetWeaponInfoByName - WeaponTable -NULL"))
	}
	
	return bIsFind;
}

bool UTT_GameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		TArray<FName> RowNames = DropItemInfoTable->GetRowNames();
		int8 i = 0;
		while (i < RowNames.Num() && !bIsFind)
		{
			FDropItem* DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			if (DropItemInfoRow->WeaponSlot.NameItem == NameItem)
			{
				bIsFind = true;
				OutInfo = *DropItemInfoRow;
			}
			i++;
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTT_GameInstance::GetDropItemInfoByWeaponName - DropItemInfoTable -NULL"))
	}
	
	return bIsFind;
}

bool UTT_GameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
		if (DropItemInfoRow)
		{
			OutInfo = *DropItemInfoRow;
			bIsFind = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTT_GameInstance::GetDropItemInfoByName - DropItemInfoTable -NULL"));
	}
	
	return bIsFind;
}

