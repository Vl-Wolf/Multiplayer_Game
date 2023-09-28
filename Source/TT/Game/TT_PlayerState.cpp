// Fill out your copyright notice in the Description page of Project Settings.


#include "TT_PlayerState.h"

ATT_PlayerState::ATT_PlayerState()
{
	WeaponSlot.Add(FWeaponSlot{"Rifle", 45, EWeaponType::RifleType, 45,120});
	WeaponSlot.Add(FWeaponSlot{"Railgun", 20, EWeaponType::RailgunType, 20, 60});
}
