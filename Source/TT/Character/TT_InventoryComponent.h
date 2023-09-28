// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT/FuncLibrary/Types.h"
#include "TT_InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAdditionalWeaponInfo, AdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAvailable, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlots, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponNotHaveRound, int32, IndexSlotWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHaveRound, int32, IndexSlotWeapon);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TT_API UTT_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UTT_InventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSwitchWeapon OnSwitchWeapon;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnAmmoChange OnAmmoChange;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponAmmoAvailable OnWeaponAmmoAvailable;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnUpdateWeaponSlots OnUpdateWeaponSlots;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponNotHaveRound OnWeaponNotHaveRound;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnWeaponHaveRound OnWeaponHaveRound;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;

	int32 MaxSlotsWeapon = 0;

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);
	bool GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType);
	bool GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType);

	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CountChangeAmmo);
	bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AvailableAmmForWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool CheckCanTakeWeapon(int32& FreeSlot);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interface")
	void TryGetWeaponToInventory_OnServer(AActor* PickUpActor, FWeaponSlot NewWeapon);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interface")
	void DropWeaponByIndex_OnServer(int32 ByIndex);

	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FWeaponSlot> GetWeaponSlots();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void InitInventory_OnServer(const TArray<FWeaponSlot>& NewWeaponSlotsInfo);

	UFUNCTION(Server, Reliable)
	void SwitchWeaponEvent_OnServer(FName WeaponName, FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot);
	UFUNCTION(NetMulticast, Reliable)
	void AmmoChangeEvent_Multicast(EWeaponType TypeWeapon, int32 Count);
	UFUNCTION(NetMulticast, Reliable)
	void WeaponAdditionalInfoChangeEvent_Multicast(int32 IndexWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo);
	UFUNCTION(NetMulticast, Reliable)
	void WeaponAmmoEmptyEvent_Multicast(EWeaponType TypeWeapon);
	UFUNCTION(NetMulticast, Reliable)
	void WeaponAmmoAvailableEvent_Multicast(EWeaponType TypeWeapon);
	UFUNCTION(NetMulticast, Reliable)
	void UpdateWeaponSlotsEvent_Multicast(int32 IndexSlotChange, FWeaponSlot NewInfo);
	UFUNCTION(NetMulticast, Reliable)
	void WeaponNotHaveRoundEvent_Multicast(int32 IndexSlotWeapon);
	UFUNCTION(NetMulticast, Reliable)
	void WeaponHaveRoundEvent_Multicast(int32 IndexSlotWeapon);
};
