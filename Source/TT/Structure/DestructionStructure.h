// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TT/Character/TT_HealthComponent.h"
#include "DestructionStructure.generated.h"

UCLASS()
class TT_API ADestructionStructure : public AActor
{
	GENERATED_BODY()

public:
	ADestructionStructure();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components)
	UStaticMeshComponent* StructureMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components)
	UBoxComponent* BoxCollision = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components)
	UTT_HealthComponent* HealthComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Explosion")
	UNiagaraSystem* ExplosionFX = nullptr;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Explosion")
	USoundBase* ExplosionSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionMaxDamage = 40.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionFallCoeff = 1.0f;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void Explosion_OnServer();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnExplosionFX_Multicast(UNiagaraSystem* FX);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnExplosionSound_Multicast(USoundBase* Sound);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radius")
	float MinRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radius")
	float MaxRadiusDamage = 400.0f;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
