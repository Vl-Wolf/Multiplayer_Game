// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TT_HealthComponent.h"
#include "TT_InventoryComponent.h"
#include "GameFramework/Character.h"
#include "TT/FuncLibrary/Types.h"
#include "TTCharacter.generated.h"

UCLASS(config=Game)
class ATTCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	virtual void Tick(float DeltaSeconds) override;
public:
	ATTCharacter();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	void InputSprintPressed();
	void InputSprintReleased();

	void InputAimPressed();
	void InputAimReleased();

protected:

	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::Walk_State;
	UPROPERTY(Replicated)
	AWeaponDefault* CurrentWeapon = nullptr;

	UPROPERTY(Replicated)
	int32 CurrentIndexWeapon = 0;

	UFUNCTION()
	void CharacterDead();
	UFUNCTION(NetMulticast, Reliable)
	void EnableRagdoll_Multicast();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	UTT_HealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	UTT_InventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FCharacterSpeed CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Death")
	TArray<UAnimMontage*> DeadAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Death")
	UParticleSystem* DeadFX;

public:

	FTimerHandle TimerHandle_RagdollTimer;

	UTT_InventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}

	void CharacterUpdate();
	void ChangeMovementState();

	void AttackEvent(bool bIsFiring);

	UFUNCTION()
	void InitWeapon(FName WeaponName, FAdditionalWeaponInfo AdditionalWeaponInfo, int32 NewCurrentIndexWeapon);

	void TryReloadWeapon();

	void WeaponFire(UAnimMontage* Anim);
	void WeaponReloadStart(UAnimMontage* Anim);
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);

	UFUNCTION(Server, Reliable)
	void TrySwitchWeapon_OnServer();

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFire_BP(UAnimMontage* Anim);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EMovementState GetMovementState();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentWeaponIndex();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsAlive();

	UFUNCTION(BlueprintNativeEvent)
	void CharacterDead_BP();

	UFUNCTION(Server, Unreliable)
	void SetActorRotationByYaw_OnServer(float Yaw);
	UFUNCTION(NetMulticast, Unreliable)
	void SetActorRotationByYaw_Multicast(float Yaw);

	UFUNCTION(Server, Reliable)
	void SetMovementState_OnServer(EMovementState NewState);
	UFUNCTION(NetMulticast, Reliable)
	void SetMovementState_Multicast(EMovementState NewState);
	UFUNCTION(Server, Reliable)
	void TryReloadWeapon_OnServer();
	UFUNCTION(NetMulticast, Reliable)
	void PlayAnim_Multicast(UAnimMontage* Anim);

private:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
};

