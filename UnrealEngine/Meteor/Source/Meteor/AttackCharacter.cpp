// Copyright IceRiver. All Rights Reserved.

#include "AttackCharacter.h"

#include "Animation/AnimInstance.h"
#include "Engine.h"
#include "GameFramework/Character.h"

#include "Meteor.h"
#include "Meteor/Common/MeteorFuncLib.h"
#include "Meteor/Components/InputBufferComponent.h"
#include "Meteor/Components/InputCommandComponent.h"
#include "Meteor/Components/CombatSystemComponent.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"

AAttackCharacter::AAttackCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MoveFwdSpeedFactor = 1.0f;

	MoveBwdSpeedFactor = 0.3f;

	MoveRightSpeedFactor = 0.3f;

	InputCommandCP = CreateDefaultSubobject<UInputCommandComponent>(TEXT("InputCommand"));
	AddTickPrerequisiteComponent(InputCommandCP);

	CombatSystemCP = CreateDefaultSubobject<UCombatSystemComponent>(TEXT("CombatSystem"));
	AddTickPrerequisiteComponent(CombatSystemCP);
	CombatSystemCP->SetInputCommandComponent(InputCommandCP);
}

void AAttackCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAttackCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (GetCharacterMovement()->MovementMode)
	{
	case EMovementMode::MOVE_Walking:
		InputCommandCP->CurrentStateType = STATE_TYPE::TYPE_STANDING;
		break;
	case EMovementMode::MOVE_Falling:
		InputCommandCP->CurrentStateType = STATE_TYPE::TYPE_AIR;
		break;
	default:
		InputCommandCP->CurrentStateType = STATE_TYPE::TYPE_STANDING;
		break;
	}
}

void AAttackCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAttackCharacter::OnJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AAttackCharacter::StopJump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAttackCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAttackCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AAttackCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AAttackCharacter::LookUp);
}

void AAttackCharacter::MoveForward(float Value)
{
	if (Value > 0.0f)
		Value *= MoveFwdSpeedFactor;
	else
		Value *= MoveBwdSpeedFactor;

	AddMovementInput(GetActorForwardVector(), Value);
}

void AAttackCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value * MoveRightSpeedFactor);
}

void AAttackCharacter::Turn(float Value)
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE)
	{
		AddControllerYawInput(Value);
	}
}

void AAttackCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AAttackCharacter::OnJump()
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE)
	{
		ACharacter::Jump();
	}
}

void AAttackCharacter::StopJump()
{
	ACharacter::StopJumping();
}

void AAttackCharacter::Test_createSection(UAnimMontage* montage)
{
	if (montage)
	{
		/*int frames = montage->GetNumberOfFrames();
		float step = frames / 30.0f;
		montage->AddAnimCompositeSection("Act1", 0.0f);
		montage->AddAnimCompositeSection("Act2", 0.1f * step);
		montage->AddAnimCompositeSection("Act3", 0.5f * step);
		montage->AddAnimCompositeSection("Act4", 0.8f * step);
		montage->DeleteAnimCompositeSection(0);*/

		//UAnimNotify_PlaySound* playSnd = CreateDefaultSubobject<UAnimNotify_PlaySound>(TEXT("playSnd"));
	}
}

void AAttackCharacter::Play_TestMontage()
{
	if (TestMtg)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(TestMtg);
	}
}
