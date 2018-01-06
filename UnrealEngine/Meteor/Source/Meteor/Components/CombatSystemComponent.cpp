// Copyright IceRiver. All Rights Reserved.

#include "CombatSystemComponent.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteor/AttackCharacter.h"
#include "Meteor/Components/InputCommandComponent.h"

UCombatSystemComponent::UCombatSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MoveType = MOVE_TYPE::MOVE_IDLE;
	NextPoseTimer = 0.0f;
	NextPoseMtg = nullptr;

	//MaxSprintWaitTime = 0.2f;
	//SprintHoldTimer = 0.0f;

	CurrentAnimFlag = EAnimFlag::Unknow;
}

void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	AttackCharacter = Cast<AAttackCharacter>(GetOwner());
}

void UCombatSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCombatSystem(DeltaTime);
}

void UCombatSystemComponent::SetInputCommandComponent(UInputCommandComponent* InputCmdCP)
{
	InputCommandCP = InputCmdCP;
	this->AddTickPrerequisiteComponent(InputCommandCP);
}

void UCombatSystemComponent::UpdateCombatSystem(float DeltaTime)
{
	switch (MoveType)
	{
	case Meteor::MOVE_TYPE::MOVE_ATTACK:
	{
		UpdateAttackState(DeltaTime);
		break;
	}
	case Meteor::MOVE_TYPE::MOVE_IDLE:
	{
		UpdateIdleState(DeltaTime);
		break;
	}
	case Meteor::MOVE_TYPE::MOVE_BEING_HIT:
		break;
	default:
		break;
	}
}

void UCombatSystemComponent::UpdateIdleState(float DeltaTime)
{
	if (InputCommandCP->StateNo != -1 && InputCommandCP->bCanAttack)
	{
		ChangeMoveType(Meteor::MOVE_TYPE::MOVE_ATTACK, MoveType);
		UpdateCombatSystem(DeltaTime);
		InputCommandCP->bHasControl = false;
		InputCommandCP->bBeginChain = false;
	}
	else
	{
		InputCommandCP->bHasControl = true;
		InputCommandCP->bBeginChain = true;
	}
}

void UCombatSystemComponent::UpdateAttackState(float DeltaTime)
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (AnimInstance)
	{
		switch (AttackAnimState)
		{
		case ATTACK_ANIM_STATE::ANIM_BEGIN:
		{
			NextPoseTimer += DeltaTime;
			if (NextPoseTimer > NextPoseTime)
			{
				ChangeAttackState(ATTACK_ANIM_STATE::ANIM_PLYING, AttackAnimState);
				UpdateAttackState(DeltaTime);
			}
			break;
		}
		case ATTACK_ANIM_STATE::ANIM_PLYING:
		{
			UAnimMontage* ActiveAnimMtg = AnimInstance->GetCurrentActiveMontage();
			if (ActiveAnimMtg)
			{
				FName sectionName = AnimInstance->Montage_GetCurrentSection(ActiveAnimMtg);
				UAnimMetaData_SectionInfo* AnimMetaData = GetAnimMetaData(ActiveAnimMtg);
				if (sectionName.IsEqual(AnimMetaData->NextPoseOut))
				{
					InputCommandCP->bBeginChain = true; // 有一帧的滞后
					if (InputCommandCP->bHasControl && InputCommandCP->bCanAttack)
					{
						InputCommandCP->bBeginChain = false;
						InputCommandCP->bHasControl = false;
						ChangeAttackState(ATTACK_ANIM_STATE::ANIM_BEGIN, AttackAnimState);
						UpdateAttackState(DeltaTime);
					}
				}
				else
				{
					float* ratio = AnimMetaData->SectionSpeeds.Find(sectionName);
					if (ratio)
					{
						AnimInstance->Montage_SetPlayRate(ActiveAnimMtg, *ratio);
					}
					else
					{
						AnimInstance->Montage_SetPlayRate(ActiveAnimMtg, 1.0f);
					}
				}
			}
			else
			{
				ChangeMoveType(MOVE_TYPE::MOVE_IDLE, MoveType);
				UpdateCombatSystem(DeltaTime);
			}
			break;
		}
		case ATTACK_ANIM_STATE::ANIM_PAUSING:
			break;
		default:
			break;
		}
	}
}

void UCombatSystemComponent::UpdateDashState(float DeltaTime)
{
	/*
	static FName Sprint_Fwd_Cmd = "U,U";
	static FName Sprint_Bwd_Cmd = "D,D";
	static FName Sprint_Left_Cmd = "L,L";
	static FName Sprint_Right_Cmd = "R,R";

	for (int32 i = 0; i < InputCmds.Num(); ++i)
	{
		const FString& cmd = InputCmds[i];
		if (cmd.Len() == 3)
		{
			if (Sprint_Fwd_Cmd == FName(*cmd))
				return SPRINT_DIRECTION::SPRINT_FWD;
			else if (Sprint_Bwd_Cmd == FName(*cmd))
				return SPRINT_DIRECTION::SPRINT_BWD;
			else if (Sprint_Left_Cmd == FName(*cmd))
				return SPRINT_DIRECTION::SPRINT_LEFT;
			else if (Sprint_Right_Cmd == FName(*cmd))
				return SPRINT_DIRECTION::SPRINT_RIGHT;
		}
	}
	return SPRINT_DIRECTION::SPRINT_IDLE;
	
	
	
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling &&
		bIsAttacking == false && bIsSprinting == false)
	{
		TArray<FString> AllInputCmds;
		AllInputCmds = InputBufferCP->GetInputCommonds();

		SPRINT_DIRECTION SprintDir = GetReadySprintDirection(AllInputCmds);
		if (SprintDir != SPRINT_DIRECTION::SPRINT_IDLE)
		{
			SprintHoldTimer += DeltaTime;
			if (SprintHoldTimer > MaxSprintWaitTime)
			{
				SprintHoldTimer = 0.0f;
				switch (SprintDir)
				{
				case AAttackCharacter::SPRINT_DIRECTION::SPRINT_FWD:
					AnimInstance->Montage_Play(SprintForwadMtg, 0.8f);
					break;
				case AAttackCharacter::SPRINT_DIRECTION::SPRINT_BWD:
					AnimInstance->Montage_Play(SprintBackwardMtg, 0.8f);
					break;
				case AAttackCharacter::SPRINT_DIRECTION::SPRINT_RIGHT:
					AnimInstance->Montage_Play(SprintRightMtg, 0.8f);
					break;
				case AAttackCharacter::SPRINT_DIRECTION::SPRINT_LEFT:
					AnimInstance->Montage_Play(SprintLeftMtg, 0.8f);
					break;
				default:
					break;
				}
			}
		}
	}*/
}

void UCombatSystemComponent::ChangeMoveType(MOVE_TYPE newType, MOVE_TYPE oldType)
{
	if (AttackCharacter)
	{
		if (newType != oldType)
		{
			if (oldType == MOVE_TYPE::MOVE_IDLE && newType == MOVE_TYPE::MOVE_ATTACK)
			{
				MoveType = newType;
				AttackAnimState = ATTACK_ANIM_STATE::ANIM_PLYING;
				UAnimInstance* AnimInstance = AttackCharacter->GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					UAnimMontage* AnimMtg = InputCommandCP->GetPoseMontage(InputCommandCP->StateNo);
					if (AnimMtg)
					{
						AnimInstance->Montage_Stop(0.0f);
						AnimInstance->Montage_Play(AnimMtg);
					}
				}
			}
			if (oldType == MOVE_TYPE::MOVE_ATTACK && newType == MOVE_TYPE::MOVE_IDLE)
			{
				MoveType = newType;
				InputCommandCP->StateNo = -1;
				InputCommandCP->RecreateStateRecord(-1);
			}
		}
	}
}

void UCombatSystemComponent::ChangeAttackState(ATTACK_ANIM_STATE newState, ATTACK_ANIM_STATE lastState)
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (AnimInstance)
	{
		if (newState != lastState)
		{
			if (lastState == ATTACK_ANIM_STATE::ANIM_PLYING && newState == ATTACK_ANIM_STATE::ANIM_BEGIN)
			{
				UAnimMontage* PoseMtg = InputCommandCP->GetPoseMontage(InputCommandCP->StateNo);
				if (PoseMtg)
				{
					UAnimMetaData_SectionInfo* AnimMetaData = GetAnimMetaData(PoseMtg);
					if (AnimMetaData)
					{
						AttackAnimState = newState;
						PoseMtg->BlendIn.SetBlendTime(NextPoseTime);
						AnimInstance->Montage_Play(PoseMtg);
						AnimInstance->Montage_JumpToSection(AnimMetaData->NextPoseIn, PoseMtg);
						AnimInstance->Montage_Pause(PoseMtg);
						NextPoseMtg = PoseMtg;
						NextPoseTimer = 0.0f;
					}
				}
			}
			else if (lastState == ATTACK_ANIM_STATE::ANIM_BEGIN && newState == ATTACK_ANIM_STATE::ANIM_PLYING)
			{
				if (NextPoseMtg)
				{
					AttackAnimState = newState;
					AnimInstance->Montage_Resume(NextPoseMtg);
					AttackCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				}
			}
		}
	}
}


UAnimMetaData_SectionInfo* UCombatSystemComponent::GetAnimMetaData(UAnimMontage* montage)
{
	if (montage)
	{
		const TArray<UAnimMetaData*> MetaDatas = montage->GetMetaData();
		for (int i = 0; i < MetaDatas.Num(); ++i)
		{
			UAnimMetaData_SectionInfo* sectionInfo = Cast<UAnimMetaData_SectionInfo>(MetaDatas[i]);
			if (sectionInfo)
				return sectionInfo;
		}
	}
	return nullptr;
}

UAnimInstance* UCombatSystemComponent::GetAnimInstance()
{
	if (AttackCharacter)
	{
		UAnimInstance* AnimInstance = AttackCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			return AnimInstance;
		}
	}
	return nullptr;
}

EAnimFlag UCombatSystemComponent::GetAnimFlag(UAnimMontage* montage)
{
	if (montage)
	{
		const TArray<UAnimMetaData*> MetaDatas = montage->GetMetaData();
		for (int i = 0; i < MetaDatas.Num(); ++i)
		{
			UAnimMetaData_Flag* flag = Cast<UAnimMetaData_Flag>(MetaDatas[i]);
			if (flag)
			{
				return flag->AnimFlag;
			}
		}
	}
	return EAnimFlag::Unknow;
}
