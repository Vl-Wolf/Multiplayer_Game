// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TT/FuncLibrary/Types.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectileDefault.generated.h"

UCLASS()
class TT_API AProjectileDefault : public AActor
{
	GENERATED_BODY()

public:
	
	AProjectileDefault();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UStaticMeshComponent* BulletMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	USphereComponent* BulletCollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UProjectileMovementComponent* ProjectileMovement = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UParticleSystemComponent* BulletFX = nullptr;

	FProjectileInfo ProjectileInfo;

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void InitProjectile(FProjectileInfo InitParam);
	UFUNCTION()
	virtual void BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
											FVector NormalImpulse, const FHitResult &Hit);
	UFUNCTION()
	void BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
											const FHitResult &SweepResult);
	UFUNCTION()
	void BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void ImpactProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void InitVisualMeshProjectile_Multicast(UStaticMesh* NewMesh, FTransform MeshRelative);
	UFUNCTION(NetMulticast, Reliable)
	void InitVisualTrailProjectile_Multicast(UParticleSystem* NewTrail, FTransform TrailRelative);
	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitDecal_Multicast(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponent, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitFX_Multicast(UParticleSystem* FX, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitSound_Multicast(USoundBase* HitSound, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void InitVelocity_Multicast(float InitSpeed, float MaxSpeed);

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	
};
