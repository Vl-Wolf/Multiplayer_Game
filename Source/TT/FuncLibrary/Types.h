// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"

/**
 * 
 */
UCLASS()
class TT_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Sprint_State UMETA(DisplayName = "Sprint State")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	RifleType UMETA(DisplayName = "Rifle"),
	PistolType UMETA(DisplayName = "Pistol"),
	RailgunType UMETA(DisplayName = "Railgun")
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float AimSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SprintSpeed = 500.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	TSubclassOf<class AProjectileDefault> Projectile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	UStaticMesh* ProjectileStaticMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	FTransform ProjectileStaticMeshOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	UParticleSystem* ProjectileTrailFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	FTransform ProjectileTrailFXOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	float ProjectileLifeTime = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	float ProjectileInitSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileSetting")
	float ProjectileMaxSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileFX")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileFX")
	USoundBase* HitSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileFX")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;
};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Aim_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Aim_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Aim_StateDispersionAimRecoil = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Aim_StateDispersionAimReduction = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Walk_StateDispersionAimMin = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	float Walk_StateDispersionAimReduction = 0.2f;
};

USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimCharacterFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimCharacterFireAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimCharacterReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimCharacterReloadAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimWeaponFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimWeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation Character")
	UAnimMontage* AnimWeaponReloadAnim = nullptr;
	
};

USTRUCT(BlueprintType)
struct FDropMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	UStaticMesh* DropMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	float DropTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	float DropMeshLifeTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	FTransform DropMeshOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	FVector DropMeshImpulseDirection = FVector(0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	float MassMesh = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	float PowerImpulse = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	float ImpulseRandomDispersion = 0.0f;
	
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Class")
	TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	float ReloadTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	int32 MaxRound = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State")
	int32 NumberProjectileByShoot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dispersion")
	FWeaponDispersion WeaponDispersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* SoundWeaponFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* SoundWeaponReload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FX")
	UParticleSystem* WeaponFireEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	FProjectileInfo ProjectileInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tarce")
	float DistanceTrace = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitDecal")
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	FAnimationWeaponInfo AnimationWeaponInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	FDropMeshInfo MagazineDrop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop Mesh")
	FDropMeshInfo ShellDrop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	float SwitchTimeWeapon = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	UTexture2D* WeaponIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	EWeaponType WeaponType = EWeaponType::RifleType;
	
};

USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponStats")
	int32 Round = 0;
};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSlot")
	FName NameItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSlot")
	FAdditionalWeaponInfo AdditionalWeaponInfo;
};

USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AmmoSlot")
	EWeaponType WeaponType = EWeaponType::RifleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AmmoSlot")
	int32 Count = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AmmoSlot")
	int32 MaxCount = 100;
};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DropWeapon")
	UStaticMesh* WeaponStaticMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DropWeapon")
	USkeletalMeshComponent* WeaponSkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DropWeapon")
	UMaterialInstance* WeaponMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DropWeapon")
	FTransform Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DropWeapon")
	FWeaponSlot WeaponSlot;
	
};

