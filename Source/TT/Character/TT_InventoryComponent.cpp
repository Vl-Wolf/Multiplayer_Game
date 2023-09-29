// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_InventoryComponent.h"
#include "TT/Game/TT_GameInstance.h"
#include "Net/UnrealNetwork.h"


UTT_InventoryComponent::UTT_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UTT_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UTT_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UTT_InventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo)
{
	bool bIsSuccess = false;

	FName ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	FAdditionalWeaponInfo ToSwitchAdditionalInfo = GetAdditionalInfoWeapon(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviousIndex, PreviousWeaponInfo);
		SwitchWeaponEvent_OnServer(ToSwitchIdWeapon, ToSwitchAdditionalInfo, IndexWeaponToChange);
		
		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AvailableAmmForWeapon = -1;
			if (CheckAmmoForWeapon(ToSwitchWeaponType, AvailableAmmForWeapon))
			{

			}
		}
		bIsSuccess = true;
	}
	return bIsSuccess;
}

FAdditionalWeaponInfo UTT_InventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo Result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				Result = WeaponSlots[i].AdditionalWeaponInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
	}

	return Result;
}

int32 UTT_InventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 Result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			Result = i;
		}
		i++;
	}
	return Result;
}

FName UTT_InventoryComponent::GetWeaponNameBySlotIndex(int32 indexSlot)
{
	FName Result;

	if (WeaponSlots.IsValidIndex(indexSlot))
	{
		Result = WeaponSlots[indexSlot].NameItem;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetWeaponNameBySlotIndex - Not Correct index Weapon  - %d"), indexSlot);
	}
	return Result;
}

bool UTT_InventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	WeaponType = EWeaponType::RifleType;
	UTT_GameInstance* MyGI = Cast<UTT_GameInstance>(GetWorld()->GetGameInstance());
	if (MyGI)
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			FWeaponInfo OutInfo;
			MyGI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}
	return bIsFind;
}

bool UTT_InventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	WeaponType = EWeaponType::RifleType;
	UTT_GameInstance* MyGI = Cast<UTT_GameInstance>(GetWorld()->GetGameInstance());
	if (MyGI)
	{
		FWeaponInfo OutInfo;
		MyGI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}
	return bIsFind;
}

void UTT_InventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalWeaponInfo = NewInfo;
				bIsFind = true;

				WeaponAdditionalInfoChangeEvent_Multicast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Found Weapon with index - %d"), IndexWeapon);			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
	}
}

void UTT_InventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CountChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].WeaponType == TypeWeapon)
		{
			WeaponSlots[i].Count += CountChangeAmmo;
			if (WeaponSlots[i].Count > WeaponSlots[i].MaxCount)
			{
				WeaponSlots[i].Count = WeaponSlots[i].MaxCount;				
			}

			AmmoChangeEvent_Multicast(WeaponSlots[i].WeaponType, WeaponSlots[i].Count);
			bIsFind = true;
		}
		i++;
	}
}

bool UTT_InventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AvailableAmmoForWeapon)
{
	AvailableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AvailableAmmoForWeapon = WeaponSlots[i].Count;
			if (WeaponSlots[i].Count > 0)
			{
				return true;
			}
		}
		i++;
	}

	if (AvailableAmmoForWeapon <= 0)
	{
		WeaponAmmoEmptyEvent_Multicast(TypeWeapon);
	}
	else
	{
		WeaponAmmoAvailableEvent_Multicast(TypeWeapon);
	}

	return false;
}

bool UTT_InventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool Result = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !Result)
	{
		if (WeaponSlots[i].WeaponType == AmmoType && WeaponSlots[i].Count < WeaponSlots[i].MaxCount)
		{
			Result = true;
		}
		i++;
	}
	return Result;
}

bool UTT_InventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}
	return bIsFreeSlot;
}

void UTT_InventoryComponent::TryGetWeaponToInventory_OnServer_Implementation(AActor* PickUpActor, FWeaponSlot NewWeapon)
{
	int32 IndexSlot = -1;
	if (CheckCanTakeWeapon(IndexSlot))
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			WeaponSlots[IndexSlot] = NewWeapon;

			UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);

			if (PickUpActor)
			{
				PickUpActor->Destroy();
			}
		}
	}
}

void UTT_InventoryComponent::DropWeaponByIndex_OnServer_Implementation(int32 ByIndex)
{
	FDropItem DropItemInfo;
	bool bIsCanDrop = false;
	int8 i = 0;
	int8 AvailableWeaponNum = 0;
	while (i < WeaponSlots.Num() && !bIsCanDrop)
	{
		if (!WeaponSlots[i].NameItem.IsNone())
		{
			AvailableWeaponNum++;
			if (AvailableWeaponNum > 1)
			{
				bIsCanDrop = true;	
			}
		}
		i++;
	}

	if (bIsCanDrop && WeaponSlots.IsValidIndex(ByIndex) && GetDropItemInfoFromInventory(ByIndex, DropItemInfo))
	{
		FWeaponSlot EmptyWeaponSlot;

		bool bIsFindWeapon = false;
		int8 j = 0;
		while (j < WeaponSlots.Num() && !bIsFindWeapon)
		{
			if (!WeaponSlots[j].NameItem.IsNone())
			{
				SwitchWeaponEvent_OnServer(WeaponSlots[j].NameItem, WeaponSlots[j].AdditionalWeaponInfo, j);
			}
			j++;
		}

		WeaponSlots[ByIndex] = EmptyWeaponSlot;
		
		UpdateWeaponSlotsEvent_Multicast(ByIndex, EmptyWeaponSlot);
		
	}
}

bool UTT_InventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool Result = false;

	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTT_GameInstance* myGI = Cast<UTT_GameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		Result = myGI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponSlot.AdditionalWeaponInfo = WeaponSlots[IndexSlot].AdditionalWeaponInfo;
		}
	}

	return Result;
}

TArray<FWeaponSlot> UTT_InventoryComponent::GetWeaponSlots()
{
	return WeaponSlots;
}

void UTT_InventoryComponent::InitInventory_OnServer_Implementation(const TArray<FWeaponSlot>& NewWeaponSlotsInfo)
{
	WeaponSlots = NewWeaponSlotsInfo;
	
	MaxSlotsWeapon = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
		{
			SwitchWeaponEvent_OnServer(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalWeaponInfo, 0);			
		}
	}
}

void UTT_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTT_InventoryComponent, WeaponSlots);
}

void UTT_InventoryComponent::SwitchWeaponEvent_OnServer_Implementation(FName WeaponName, FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot)
{
	OnSwitchWeapon.Broadcast(WeaponName, AdditionalInfo, IndexSlot);
}

void UTT_InventoryComponent::WeaponAdditionalInfoChangeEvent_Multicast_Implementation(int32 IndexWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo)
{
	OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, AdditionalWeaponInfo);
}

void UTT_InventoryComponent::AmmoChangeEvent_Multicast_Implementation(EWeaponType TypeWeapon, int32 Count)
{
	OnAmmoChange.Broadcast(TypeWeapon, Count);
}

void UTT_InventoryComponent::WeaponAmmoEmptyEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
}

void UTT_InventoryComponent::WeaponAmmoAvailableEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoAvailable.Broadcast(TypeWeapon);
}

void UTT_InventoryComponent::UpdateWeaponSlotsEvent_Multicast_Implementation(int32 IndexSlotChange, FWeaponSlot NewInfo)
{
	OnUpdateWeaponSlots.Broadcast(IndexSlotChange, NewInfo);
}

void UTT_InventoryComponent::WeaponNotHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponNotHaveRound.Broadcast(IndexSlotWeapon);
}

void UTT_InventoryComponent::WeaponHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponHaveRound.Broadcast(IndexSlotWeapon);
}
