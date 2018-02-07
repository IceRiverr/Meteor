// Copyright IceRiver. All Rights Reserved.

#include "NetCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "UnrealNetwork.h"


// Sets default values
ANetCharacter::ANetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	NetUpdateFrequency = 30.0f;

	// 

	bAttackFlag = false;
}

void ANetCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetCharacter, bAttackFlag);
}

// Called when the game starts or when spawned
void ANetCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ANetCharacter::StartAttack);

	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ANetCharacter::StopAttack);
}

void ANetCharacter::StartAttack()
{
	PlayAttack();
	SERVER_Attack(true);
}

void ANetCharacter::StopAttack()
{
	SERVER_Attack(false);
}

void ANetCharacter::PlayAttack()
{
	if (DaoAttackMtg)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DaoAttackMtg);
		}
	}
}

void ANetCharacter::SERVER_Attack_Implementation(uint32 bAttack)
{
	bAttackFlag = bAttack;
	if (Role == ROLE_Authority)
	{
		OnRep_SetAttackFlag();
	}
}

bool ANetCharacter::SERVER_Attack_Validate(uint32 bAttack)
{
	return true;
}

void ANetCharacter::OnRep_SetAttackFlag()
{
	if (bAttackFlag)
	{
		PlayAttack();
	}
}
