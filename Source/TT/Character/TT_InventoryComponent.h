// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT/FuncLibrary/Types.h"
#include "TT_InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TT_API UTT_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTT_InventoryComponent();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnSwitchWeapon OnSwitchWeapon;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TArray<FWeaponSlot> WeaponSlot;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TArray<FAmmoSlot> AmmoSlot;

	UFUNCTION(Server, Reliable, Category="Imventory")
	void InitInventory_OnServer(const TArray<FWeaponSlot> &NewWeaponSlotInfo, const TArray<FAmmoSlot> &NewAmmoSlotInfo);

	int8 MaxSlotWeapon = 0;

	UFUNCTION(Server, Reliable)
	void SwitchWeaponEvent_OnServer(FName WeaponName, FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
