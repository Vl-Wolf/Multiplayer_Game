// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_InventoryComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UTT_InventoryComponent::UTT_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTT_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTT_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTT_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTT_InventoryComponent, WeaponSlot);
	DOREPLIFETIME(UTT_InventoryComponent, AmmoSlot);
}

void UTT_InventoryComponent::SwitchWeaponEvent_OnServer_Implementation(FName WeaponName,
                                                                       FAdditionalWeaponInfo AdditionalInfo,
                                                                       int32 IndexSlot)
{
	OnSwitchWeapon.Broadcast(WeaponName, AdditionalInfo, IndexSlot);
}

void UTT_InventoryComponent::InitInventory_OnServer_Implementation(const TArray<FWeaponSlot>& NewWeaponSlotInfo,
                                                                   const TArray<FAmmoSlot>& NewAmmoSlotInfo)
{
	WeaponSlot = NewWeaponSlotInfo;
	AmmoSlot = NewAmmoSlotInfo;

	MaxSlotWeapon = WeaponSlot.Num();

	if (WeaponSlot.IsValidIndex(0))
	{
		if (!WeaponSlot[0].NameItem.IsNone())
		{
			SwitchWeaponEvent_OnServer(WeaponSlot[0].NameItem,  WeaponSlot[0].AdditionalWeaponInfo, 0);
		}
	}
}

