// Copyright Epic Games, Inc. All Rights Reserved.

#include "TTCharacter.h"

#include "TT_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "TT/Game/TT_GameInstance.h"
#include "TT/Weapon/WeaponDefault.h"

//////////////////////////////////////////////////////////////////////////
// ATTCharacter

ATTCharacter::ATTCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 100.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Camera does not rotate relative to arm

	Health = CreateDefaultSubobject<UTT_HealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UTT_InventoryComponent>(TEXT("InventoryComponent"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	if (Health)
	{
		Health->OnDead.AddDynamic(this, &ATTCharacter::CharacterDead);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATTCharacter::InitWeapon);
	}
	
}

void ATTCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATTCharacter::InputSprintPressed()
{
	ChangeMovementState();
}

void ATTCharacter::InputSprintReleased()
{
	ChangeMovementState();
}

void ATTCharacter::InputAimPressed()
{
	ChangeMovementState();
	CameraBoom->TargetArmLength = 30.0f;
}

void ATTCharacter::InputAimReleased()
{
	ChangeMovementState();
	CameraBoom->TargetArmLength = 100.0f;
}

EMovementState ATTCharacter::GetMovementState()
{
	return MovementState;
}

void ATTCharacter::CharacterDead()
{
	CharacterDead_BP();

	if (HasAuthority())
	{
		float TimeAnim = 0.0f;
		int8 Random = FMath::RandHelper(DeadAnim.Num());
		if (DeadAnim.IsValidIndex(Random) && DeadAnim[Random] && GetMesh() && GetMesh()->GetAnimInstance())
		{
			TimeAnim = DeadAnim[Random]->GetPlayLength();
			PlayAnim_Multicast(DeadAnim[Random]);
		}

		if (GetController())
		{
			GetController()->UnPossess();
		}

		float DecreaseAniTimer = FMath::FRandRange(0.2f, 1.0f);

		GetWorldTimerManager().SetTimer(TimerHandle_RagdollTimer, this, &ATTCharacter::EnableRagdoll_Multicast,
									TimeAnim - DecreaseAniTimer, false);

		SetLifeSpan(20.0f);
		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->SetLifeSpan(20.0f);
		}
	}
	else
	{
		AttackEvent(false);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void ATTCharacter::EnableRagdoll_Multicast_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATTCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Health && Health->GetIsAlive())
	{
		Health->ChangeHealthValue_OnServer(-DamageAmount);
	}

	return ActualDamage;
}


void ATTCharacter::CharacterUpdate()
{
	float ResultSpeed = 300.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResultSpeed = CharacterSpeed.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResultSpeed = CharacterSpeed.WalkSpeed;
		break;
	case EMovementState::Sprint_State:
		ResultSpeed = CharacterSpeed.SprintSpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResultSpeed;
}

void ATTCharacter::ChangeMovementState()
{
	EMovementState NewState = EMovementState::Walk_State;

	ATT_PlayerController* MyController = Cast<ATT_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (MyController)
	{
		if (!MyController->bIsSprinting && !MyController->bIsAiming)
		{
			NewState = EMovementState::Walk_State;
		}
		else
		{
			if (MyController->bIsSprinting)
			{
				NewState = EMovementState::Sprint_State;
			}
			else
			{
				if (!MyController->bIsSprinting && MyController->bIsAiming)
				{
					NewState = EMovementState::Aim_State;
				}
			}
		}
	}

	SetMovementState_OnServer(NewState);
}

void ATTCharacter::AttackEvent(bool bIsFiring)
{
	if (Health && Health->GetIsAlive())
	{
		AWeaponDefault* MyWeapon = GetCurrentWeapon();
		if (MyWeapon)
		{
			MyWeapon->SetWeaponStateFire_OnServer(bIsFiring);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATTCharacter::AttackCharEvent - CurrentWeapon -NULL"));
		}
	}
}

void ATTCharacter::InitWeapon(FName WeaponName, FAdditionalWeaponInfo AdditionalWeaponInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTT_GameInstance* MyGI = Cast<UTT_GameInstance>(GetGameInstance());
	if (MyGI)
	{
		FWeaponInfo MyWeaponInfo;
		if (MyGI->GetWeaponInfoByName(WeaponName, MyWeaponInfo))
		{
			if (MyWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);
				FActorSpawnParameters SpawnParameters;

				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParameters.Owner = this;
				SpawnParameters.Instigator = GetInstigator();

				AWeaponDefault* MyWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(MyWeaponInfo.WeaponClass,
																	&SpawnLocation, &SpawnRotation, SpawnParameters));

				if (MyWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					MyWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocket"));
					CurrentWeapon = MyWeapon;

					MyWeapon->WeaponName = WeaponName;
					MyWeapon->WeaponInfo = MyWeaponInfo;
					MyWeapon->ReloadTimer = MyWeaponInfo.ReloadTime;
					MyWeapon->AdditionalWeaponInfo = AdditionalWeaponInfo;

					CurrentIndexWeapon = NewCurrentIndexWeapon;

					//move to tick()
					MyWeapon->SetActorRotation(FollowCamera->GetForwardVector().Rotation());

					MyWeapon->OnWeaponReloadStart.AddDynamic(this, &ATTCharacter::WeaponReloadStart);
					MyWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATTCharacter::WeaponReloadEnd);
					MyWeapon->OnWeaponFire.AddDynamic(this, &ATTCharacter::WeaponFire);

					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}

					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAvailable.Broadcast(MyWeapon->WeaponInfo.WeaponType);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATTCharacter::InitWeapon - Weapon not found in table -NULL"))
		}
	}
}

void ATTCharacter::TryReloadWeapon()
{
	if (Health && Health->GetIsAlive() && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		TryReloadWeapon_OnServer();
	}
}

void ATTCharacter::WeaponFire(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}
	
	WeaponFire_BP(Anim);
}

void ATTCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATTCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe)
{
	if (InventoryComponent&&  CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponInfo.WeaponType, AmmoSafe);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponReloadEnd_BP(bIsSuccess);
}

void ATTCharacter::TrySwitchWeapon()
{
	int32 Index = GetCurrentWeaponIndex();
	if (Index == 0)
	{
		TrySwitchWeapon_OnServer(1);
	}
	if (Index == 1)
	{
		TrySwitchWeapon_OnServer(0);
	}
}

void ATTCharacter::TrySwitchWeapon_OnServer_Implementation(int32 ToIndex)
{
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
	{
		if (CurrentIndexWeapon != ToIndex && InventoryComponent)
		{
			int32 OldIndex = CurrentIndexWeapon;
			FAdditionalWeaponInfo OldInfo;

			if (CurrentWeapon)
			{
				OldInfo = CurrentWeapon->AdditionalWeaponInfo;
				if (CurrentWeapon->WeaponReloading)
				{
					CurrentWeapon->CancelReload();
				}
			}

			InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
		}
	}
}

void ATTCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
}

void ATTCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
}

void ATTCharacter::WeaponFire_BP_Implementation(UAnimMontage* Anim)
{
}

AWeaponDefault* ATTCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

int32 ATTCharacter::GetCurrentWeaponIndex()
{
	return CurrentIndexWeapon;
}

bool ATTCharacter::GetIsAlive()
{
	bool Result = false;
	if (Health)
	{
		Result = Health->GetIsAlive();
	}

	return Result;
}


void ATTCharacter::CharacterDead_BP_Implementation()
{
}

void ATTCharacter::SetActorRotationByYaw_OnServer_Implementation(float Yaw)
{
	SetActorRotationByYaw_Multicast(Yaw);
}

void ATTCharacter::SetActorRotationByYaw_Multicast_Implementation(float Yaw)
{
	if (Controller && !Controller->IsLocalController())
	{
		SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
	}
}

void ATTCharacter::SetMovementState_OnServer_Implementation(EMovementState NewState)
{
	SetMovementState_Multicast(NewState);
}

void ATTCharacter::SetMovementState_Multicast_Implementation(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();
}

void ATTCharacter::TryReloadWeapon_OnServer_Implementation()
{
	if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponInfo.MaxRound && CurrentWeapon->CheckCanWeaponReload())
	{
		CurrentWeapon->InitReload();
	}
}

void ATTCharacter::PlayAnim_Multicast_Implementation(UAnimMontage* Anim)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Anim);
	}
}

void ATTCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTCharacter, MovementState);
	DOREPLIFETIME(ATTCharacter, CurrentWeapon);
	DOREPLIFETIME(ATTCharacter, CurrentIndexWeapon);
	
}

void ATTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
