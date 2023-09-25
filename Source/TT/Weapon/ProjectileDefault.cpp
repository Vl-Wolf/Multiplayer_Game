// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault.h"

#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "NiagaraComponent.h"


// Sets default values
AProjectileDefault::AProjectileDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	BulletCollisionSphere->SetSphereRadius(16.0f);
	BulletCollisionSphere->bReturnMaterialOnMove = true;
	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagara"));
	BulletNiagara->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
}

void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);	
}

void AProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

	if (InitParam.ProjectileStaticMesh)
	{
		InitVisualMeshProjectile_Multicast(InitParam.ProjectileStaticMesh, InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFX)
	{
		InitVisualTrailProjectile_Multicast(InitParam.ProjectileTrailFX, InitParam.ProjectileTrailFXOffset);
	}
	else
	{
		BulletNiagara->DestroyComponent();
	}

	InitVelocity_Multicast(InitParam.ProjectileInitSpeed, InitParam.ProjectileMaxSpeed);

	ProjectileInfo = InitParam;
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface MySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		if (ProjectileInfo.HitDecals.Contains(MySurfaceType))
		{
			UMaterialInterface* MyMaterial = ProjectileInfo.HitDecals[MySurfaceType];

			if (MyMaterial && OtherComp)
			{
				SpawnHitDecal_Multicast(MyMaterial, OtherComp, Hit);
			}
		}

		if (ProjectileInfo.HitFXs.Contains(MySurfaceType))
		{
			UNiagaraSystem* MyParticle = ProjectileInfo.HitFXs[MySurfaceType];

			if (MyParticle)
			{
				SpawnHitFX_Multicast(MyParticle, Hit);
			}
		}

		if (ProjectileInfo.HitSound)
		{
			SpawnHitSound_Multicast(ProjectileInfo.HitSound, Hit);
		}
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileInfo.ProjectileDamage, Hit.TraceStart, Hit,
											GetInstigatorController(), this, nullptr);

	UAISense_Damage::ReportDamageEvent(GetWorld(), Hit.GetActor(), GetInstigator(), ProjectileInfo.ProjectileDamage,
											Hit.Location, Hit.Location);

	ImpactProjectile();
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AProjectileDefault::ImpactProjectile()
{
	this->Destroy();
}

void AProjectileDefault::InitVisualMeshProjectile_Multicast_Implementation(UStaticMesh* NewMesh,
	FTransform MeshRelative)
{
	BulletMesh->SetStaticMesh(NewMesh);
	BulletMesh->SetRelativeTransform(MeshRelative);
}

void AProjectileDefault::InitVisualTrailProjectile_Multicast_Implementation(UNiagaraSystem* NewTrail,
	FTransform TrailRelative)
{
	/*BulletFX->SetTemplate(NewTrail);
	BulletFX->SetRelativeTransform(TrailRelative);*/

	//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NewTrail, TrailRelative.GetLocation(), TrailRelative.Rotator(), FVector(1.0f));

	BulletNiagara->SetAsset(NewTrail);
	BulletNiagara->SetRelativeTransform(TrailRelative);
}

void AProjectileDefault::SpawnHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial,
	UPrimitiveComponent* OtherComponent, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), OtherComponent, NAME_None,
									HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(),
									EAttachLocation::KeepWorldPosition, 10.0f);
}

void AProjectileDefault::SpawnHitFX_Multicast_Implementation(UNiagaraSystem* FX, FHitResult HitResult)
{
	/*UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FX, FTransform(HitResult.ImpactNormal.Rotation(),
									HitResult.ImpactPoint, FVector(1.0f)));*/
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FX, HitResult.ImpactPoint,
														HitResult.ImpactNormal.Rotation(), FVector(1.0f));
}

void AProjectileDefault::SpawnHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}

void AProjectileDefault::InitVelocity_Multicast_Implementation(float InitSpeed, float MaxSpeed)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = GetActorForwardVector() * InitSpeed;
		ProjectileMovement->MaxSpeed = MaxSpeed;
		ProjectileMovement->InitialSpeed = InitSpeed;
	}
}

void AProjectileDefault::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	Super::PostNetReceiveVelocity(NewVelocity);

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = NewVelocity;
	}
}

