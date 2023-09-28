// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "TT/FuncLibrary/Types.h"
#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, AnimReloadStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd, bool, bIsSucces, int32, AmmoSafe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFire, UAnimMontage*, AnimFireCharacter);

UCLASS()
class TT_API AWeaponDefault : public AActor
{
	GENERATED_BODY()

public:
	
	AWeaponDefault();

	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponFire OnWeaponFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Components")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Components")
	USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Components")
	UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Components")
	UArrowComponent* ShootLocation = nullptr;

	UPROPERTY(VisibleAnywhere)
	FWeaponInfo WeaponInfo;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="WeaponInfo")
	FAdditionalWeaponInfo AdditionalWeaponInfo;

	float FireTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reload")
	float ReloadTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	bool WeaponFiring = false;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Reload")
	bool WeaponReloading = false;

	bool WeaponAiming = false;
	bool BlockFire = false;

	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 0.0f;
	float CurrentDispersionMin = 0.0f;
	float CurrentDispersionRecoil = 0.0f;
	float CurrentDispersionReduction = 0.0f;

	bool DropMagazineFlag = false;
	float DropMagazineTimer = -1.0f;

	bool DropShellFlag = false;
	float DropShellTImer = -1.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FVector ShootEndLocation = FVector(0);

protected:
	
	virtual void BeginPlay() override;

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	void WeaponInit();

	UFUNCTION()
	void Fire();

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponStateFire_OnServer(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();
	
	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	UFUNCTION()
	void InitReload();
	void FinishReload();
	void CancelReload();

	bool CheckCanWeaponReload();

	int8 GetAvailableAmmoForReload();

	UFUNCTION(Server, Reliable)
	void InitDropMesh_OnServer(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime,
								float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 100.0f;
	
	UFUNCTION(NetMulticast, Unreliable)
	void AnimWeaponStart_Multicast(UAnimMontage* Anim);
	UFUNCTION(NetMulticast, Unreliable)
	void ShellDropFire_Multicast(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime,
								float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion,
								FVector LocalDir);
	UFUNCTION(NetMulticast, Unreliable)
	void FXWeaponFire_Multicast(UNiagaraSystem* FXFire, USoundBase* SoundFire);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitDecal_Multicast(UMaterialInterface* DecalMaterial, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitFX_Multicast(UNiagaraSystem* FX, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitSound_Multicast(USoundBase* HitSound, FHitResult HitResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
