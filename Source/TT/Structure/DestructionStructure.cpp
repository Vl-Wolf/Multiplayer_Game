// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructionStructure.h"

#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ADestructionStructure::ADestructionStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	BoxCollision->SetCanEverAffectNavigation(false);

	RootComponent = BoxCollision;

	StructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Structure Mesh"));
	StructureMesh->SetupAttachment(RootComponent);
	StructureMesh->SetCanEverAffectNavigation(false);

	HealthComponent = CreateDefaultSubobject<UTT_HealthComponent>(TEXT("HealthComponent"));

	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &ADestructionStructure::Explosion_OnServer);
	}
}

float ADestructionStructure::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && HealthComponent->GetIsAlive())
	{
		HealthComponent->ChangeHealthValue_OnServer(-DamageAmount);
	}

	return ActualDamage;
}

void ADestructionStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADestructionStructure, ExplosionFX);
	DOREPLIFETIME(ADestructionStructure, ExplosionSound);
}

void ADestructionStructure::Explosion_OnServer_Implementation()
{
	if (ShowDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), MinRadiusDamage, 20, FColor::Green,
			false, 5.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), MaxRadiusDamage, 20, FColor::Red,
			false, 5.0f);
	}

	if (ExplosionFX)
	{
		SpawnExplosionFX_Multicast(ExplosionFX);
	}
	if (ExplosionSound)
	{
		SpawnExplosionSound_Multicast(ExplosionSound);
	}

	TArray<AActor*> IgnoredActors;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ExplosionMaxDamage,
														ExplosionMaxDamage * 0.2f,
														GetActorLocation(),
														MinRadiusDamage,
														MaxRadiusDamage,
														ExplosionFallCoeff,
														nullptr,
														IgnoredActors, this, nullptr);

	this->Destroy();
}

void ADestructionStructure::SpawnExplosionSound_Multicast_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
}

void ADestructionStructure::SpawnExplosionFX_Multicast_Implementation(UNiagaraSystem* FX)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
}
