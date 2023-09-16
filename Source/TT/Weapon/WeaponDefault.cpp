// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"

#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AWeaponDefault::AWeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollisison"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	WeaponInit();
}

void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FireTick(DeltaTime);
		ReloadTick(DeltaTime);
		DispersionTick(DeltaTime);
		DropTick(DeltaTime);
		ShellDropTick(DeltaTime);
	}
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if(WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTimer < 0.0f)
		{
			Fire();
		}
		else
		{
			FireTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if(WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
			{
				CurrentDispersion -= CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion += CurrentDispersionReduction;
			}
		}
		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
	}
}

void AWeaponDefault::DropTick(float DeltaTime)
{
	if (DropMagazineFlag)
	{
		if (DropMagazineTimer < 0.0f)
		{
			DropMagazineFlag = false;
			InitDropMesh_OnServer(WeaponInfo.MagazineDrop.DropMesh, WeaponInfo.MagazineDrop.DropMeshOffset,
									WeaponInfo.MagazineDrop.DropMeshImpulseDirection, WeaponInfo.MagazineDrop.DropTime,
									WeaponInfo.MagazineDrop.DropMeshLifeTime, WeaponInfo.MagazineDrop.MassMesh,
									WeaponInfo.MagazineDrop.PowerImpulse, WeaponInfo.MagazineDrop.ImpulseRandomDispersion);
		}
		else
		{
			DropMagazineTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTImer < 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh_OnServer(WeaponInfo.ShellDrop.DropMesh, WeaponInfo.ShellDrop.DropMeshOffset,
									WeaponInfo.ShellDrop.DropMeshImpulseDirection, WeaponInfo.ShellDrop.DropTime,
									WeaponInfo.ShellDrop.DropMeshLifeTime, WeaponInfo.ShellDrop.MassMesh,
									WeaponInfo.ShellDrop.PowerImpulse, WeaponInfo.ShellDrop.ImpulseRandomDispersion);
		}
		else
		{
			DropShellTImer -= DeltaTime;
		}
	}
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}

	UpdateStateWeapon_OnServer(EMovementState::Walk_State);
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponInfo.AnimationWeaponInfo.AnimCharacterFireAim;
	}
	else
	{
		AnimPlay = WeaponInfo.AnimationWeaponInfo.AnimCharacterFire;
	}

	if (WeaponInfo.AnimationWeaponInfo.AnimWeaponFire)
	{
		AnimWeaponStart_Multicast(WeaponInfo.AnimationWeaponInfo.AnimWeaponFire);
	}

	if (WeaponInfo.ShellDrop.DropMesh)
	{
		if (WeaponInfo.ShellDrop.DropTime < 0.0f)
		{
			InitDropMesh_OnServer(WeaponInfo.ShellDrop.DropMesh, WeaponInfo.ShellDrop.DropMeshOffset,
									WeaponInfo.ShellDrop.DropMeshImpulseDirection, WeaponInfo.ShellDrop.DropTime,
									WeaponInfo.ShellDrop.DropMeshLifeTime, WeaponInfo.ShellDrop.MassMesh,
									WeaponInfo.ShellDrop.PowerImpulse, WeaponInfo.ShellDrop.ImpulseRandomDispersion);
		}
		else
		{
			DropShellFlag = true;
			DropShellTImer = WeaponInfo.ShellDrop.DropTime;
		}
	}

	FireTimer = WeaponInfo.RateOfFire;
	AdditionalWeaponInfo.Round -= 1;
	ChangeDispersionByShoot();

	OnWeaponFire.Broadcast(AnimPlay);

	FXWeaponFire_Multicast(WeaponInfo.WeaponFireEffect, WeaponInfo.SoundWeaponFire);

	int8 NumberProjectile = GetNumberProjectileByShoot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		FVector EndLocation;

		for (int8 i = 0; i < NumberProjectile; i++)
		{
			EndLocation = GetFireEndLocation();

			if (ProjectileInfo.Projectile)
			{
				FVector Dir = EndLocation - SpawnLocation;
				Dir.Normalize();
				FMatrix MyMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
				SpawnRotation = MyMatrix.Rotator();

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParameters.Owner = GetOwner();
				SpawnParameters.Instigator = GetInstigator();

				AProjectileDefault* MyProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParameters));
				if (MyProjectile)
				{
					MyProjectile->InitProjectile(WeaponInfo.ProjectileInfo);
				}
			}
			else
			{
				FHitResult HitResult;
				TArray<AActor*> Actors;
				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation +
									ShootLocation->GetForwardVector() * WeaponInfo.DistanceTrace, FColor::Black,
									false, 5.0f, '\000', 0.5f);
				}
				else
				{
					EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;
				}

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponInfo.DistanceTrace,
														TraceTypeQuery4, false, Actors,
														EDrawDebugTrace::ForDuration,HitResult, true,
														FLinearColor::Red, FLinearColor::Green,
														5.0f);

				if (HitResult.GetActor() && HitResult.PhysMaterial.IsValid())
				{
					EPhysicalSurface MySurfaceType = UGameplayStatics::GetSurfaceType(HitResult);

					if (WeaponInfo.ProjectileInfo.HitDecals.Contains(MySurfaceType))
					{
						UMaterialInterface* MyMaterial = WeaponInfo.ProjectileInfo.HitDecals[MySurfaceType];

						if (MyMaterial && HitResult.GetComponent())
						{
							SpawnTraceHitDecal_Multicast(MyMaterial, HitResult);
						}
					}

					if (WeaponInfo.ProjectileInfo.HitFXs.Contains(MySurfaceType))
					{
						UParticleSystem* MyParticle = WeaponInfo.ProjectileInfo.HitFXs[MySurfaceType];

						if (MyParticle)
						{
							SpawnTraceHitFX_Multicast(MyParticle, HitResult);
						}
					}

					if (WeaponInfo.ProjectileInfo.HitSound)
					{
						SpawnTraceHitSound_Multicast(WeaponInfo.ProjectileInfo.HitSound, HitResult);
					}

					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), WeaponInfo.ProjectileInfo.ProjectileDamage,
														HitResult.TraceStart, HitResult, GetInstigatorController(),
														this, nullptr);
				}
			}
		}
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
		{
			InitReload();
		}
	}
}

void AWeaponDefault::SetWeaponStateFire_OnServer_Implementation(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;
	}
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return  WeaponInfo.ProjectileInfo;
}

void AWeaponDefault::UpdateStateWeapon_OnServer_Implementation(EMovementState NewMovementState)
{
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponInfo.WeaponDispersion.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.WeaponDispersion.Aim_StateDispersionAimReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = false;
		SetWeaponStateFire_OnServer(false);
		break;
	default:
		break;
	}
}

void AWeaponDefault::ChangeDispersionByShoot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI/180.0f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.0f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - EndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation),
							WeaponInfo.DistanceTrace, GetCurrentDispersion() * PI / 180.0f,
							GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false,
							0.1f, '\000', 1.0f);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector() * 20000.0f);
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() - ShootEndLocation,
							WeaponInfo.DistanceTrace, GetCurrentDispersion() * PI / 180.0f,
							GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false,
							0.1f, '\000', 1.0f);
		}
	}

	if(ShowDebug)
	{
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() +
						ShootLocation->GetComponentLocation() * 500.0f, FColor::Cyan, false, 5.0f,
						'\000', 0.5f);

		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red,
			false, 5.0f, '\000', 0.5f);

		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black,
			false, 5.0f,'\000', 0.5f);
	}

	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShoot() const
{
	return WeaponInfo.NumberProjectileByShoot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponInfo.ReloadTime;

	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponInfo.AnimationWeaponInfo.AnimCharacterReloadAnim;
	}
	else
	{
		AnimPlay = WeaponInfo.AnimationWeaponInfo.AnimCharacterReload;
	}

	OnWeaponReloadStart.Broadcast(AnimPlay);

	UAnimMontage* AnimWeaponPlay = nullptr;

	if (WeaponAiming)
	{
		AnimWeaponPlay = WeaponInfo.AnimationWeaponInfo.AnimWeaponReloadAnim;
	}
	else
	{
		AnimWeaponPlay = WeaponInfo.AnimationWeaponInfo.AnimWeaponReload;
	}

	if (WeaponInfo.AnimationWeaponInfo.AnimCharacterReload && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		AnimWeaponStart_Multicast(AnimWeaponPlay);
	}

	if (WeaponInfo.MagazineDrop.DropMesh)
	{
		DropMagazineFlag = true;
		DropMagazineTimer = WeaponInfo.MagazineDrop.DropTime;
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int8 AvailableAmmoFromInventory = GetAvailableAmmoForReload();
	int8 AmmoNeedTakeFromInventory;
	int8 NeedToReload = WeaponInfo.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AvailableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round += AvailableAmmoFromInventory;
		AmmoNeedTakeFromInventory = AvailableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInventory = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInventory);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropMagazineFlag = false;
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool Result = true;

	if (GetOwner())
	{
		
	}
	
	return Result;
}

int8 AWeaponDefault::GetAvailableAmmoForReload()
{
	int8 AvailableAmmoForWeapon = WeaponInfo.MaxRound;

	return AvailableAmmoForWeapon;
}

void AWeaponDefault::InitDropMesh_OnServer_Implementation(UStaticMesh* DropMesh, FTransform Offset,
	FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse,
	float ImpulseRandomDispersion)
{
	if (DropMesh)
	{
		FTransform Transform;
		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() *
							Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());
		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());

		ShellDropFire_Multicast(DropMesh, Transform, DropImpulseDirection, DropTime, LifeTimeMesh, MassMesh, PowerImpulse,
									ImpulseRandomDispersion, LocalDir);
	}
}

void AWeaponDefault::UpdateWeaponByCharacterMovementState_OnServer_Implementation(FVector NewShootEndLocation,
	bool NewShouldReduceDispersion)
{
	ShootEndLocation = NewShootEndLocation;
	ShouldReduceDispersion = NewShouldReduceDispersion;
}

void AWeaponDefault::AnimWeaponStart_Multicast_Implementation(UAnimMontage* Anim)
{
	if (Anim && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(Anim);
	}
}

void AWeaponDefault::ShellDropFire_Multicast_Implementation(UStaticMesh* DropMesh, FTransform Offset,
	FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse,
	float ImpulseRandomDispersion, FVector LocalDir)
{
	AStaticMeshActor* NewActor = nullptr;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParameters.Owner = this;

	NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Offset, SpawnParameters);

	if (NewActor && NewActor->GetStaticMeshComponent())
	{
		NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoredOnlyPawn"));
		NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		NewActor->SetActorTickEnabled(false);
		NewActor->InitialLifeSpan = LifeTimeMesh;
		NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
		NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

		if (MassMesh > 0.0f)
		{
			NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, MassMesh, true);
		}

		if (!DropImpulseDirection.IsNearlyZero())
		{
			FVector FinalDir;
			LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

			if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
			{
				FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
			}
			FinalDir.GetSafeNormal(0.0001);
			NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
		}
	}
}

void AWeaponDefault::FXWeaponFire_Multicast_Implementation(UParticleSystem* FXFire, USoundBase* SoundFire)
{
	if (SoundFire)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundFire, ShootLocation->GetComponentLocation());
	}
	if (FXFire)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, ShootLocation->GetComponentTransform());
	}
}

void AWeaponDefault::SpawnTraceHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial,
	FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), HitResult.GetComponent(), NAME_None,
											HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(),
											EAttachLocation::KeepWorldPosition, 10.0f);
}

void AWeaponDefault::SpawnTraceHitFX_Multicast_Implementation(UParticleSystem* FX, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FX, FTransform(HitResult.ImpactNormal.Rotation(),
									HitResult.ImpactPoint, FVector(1.0f)));
}

void AWeaponDefault::SpawnTraceHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}

void AWeaponDefault::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponDefault, AdditionalWeaponInfo);
	DOREPLIFETIME(AWeaponDefault, WeaponReloading);
	DOREPLIFETIME(AWeaponDefault, ShootEndLocation);
}

