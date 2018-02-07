// Copyright IceRiver. All Rights Reserved.

#include "CombatSystemComponent.h"

#include "Engine.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AttackCharacter.h"
#include "Components/InputCommandComponent.h"
#include "Common/MeteorFuncLib.h"

UCombatSystemComponent::UCombatSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;

	ActoinStateFactory = new FMeteorActionStateFactory();

	CurrentActionState = nullptr;

	FrameLinkTime = 0.0f;
}

void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	AttackCharacter = Cast<AAttackCharacter>(GetOwner());

	CurrentActionState = ActoinStateFactory->CreateIdleState(AttackCharacter);
}

void UCombatSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentActionState)
	{
		CurrentActionState->Execute(DeltaTime);

		if (GetMoveType() == MOVE_TYPE::MOVE_JUMP)
		{
			InputCommandCP->CurrentStateType = STATE_TYPE::TYPE_AIR;
		}
		else
		{
			InputCommandCP->CurrentStateType = STATE_TYPE::TYPE_STANDING;
		}
	}
}

void UCombatSystemComponent::SetInputCommandComponent(UInputCommandComponent* InputCmdCP)
{
	InputCommandCP = InputCmdCP;
	this->AddTickPrerequisiteComponent(InputCommandCP);
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

void UCombatSystemComponent::ChangeActionState(FActionState* newState)
{
	CurrentActionState->Exit();
	CurrentActionState = newState;
	CurrentActionState->Enter();
}

MOVE_TYPE UCombatSystemComponent::GetMoveType() const
{
	if (CurrentActionState)
	{
		return CurrentActionState->GetMoveType();
	}
	return MOVE_TYPE::MOVE_IDLE;
}


UAnimMontage* UCombatSystemComponent::GetPoseMontage(int PoseIndex)
{
	if (PoseMontageDB)
	{
		FString poseStr = FString::FromInt(PoseIndex);
		FPoseActionMontage* PoseAction = PoseMontageDB->FindRow<FPoseActionMontage>(FName(*poseStr), "");
		if (PoseAction)
		{
			UAnimMontage* poseMtg = PoseAction->PoseMontage.Get();
			if (poseMtg == nullptr)
			{
				
				poseMtg = UMeteorFuncLib::GetMeteorSingleton()->StreamMgr.LoadSynchronous(PoseAction->PoseMontage);
			}
			return poseMtg;
		}
	}
	return nullptr;
}

FPoseAttackInfo* UCombatSystemComponent::GetPoseAttackInfo(int PoseIndex, int AttackIndex)
{
	if (PoseMontageDB)
	{
		FString AttackInfoStr = FString::FromInt(PoseIndex).Append(".").Append(FString::FromInt(AttackIndex));
		
		FPoseAttackInfo* AttackInfo = PoseAttackInfoDB->FindRow<FPoseAttackInfo>(FName(*AttackInfoStr), "");
		if (AttackInfo)
		{
			return AttackInfo;
		}
	}
	return nullptr;
}

TArray<int> UCombatSystemComponent::GetPoseAttackBones(int PoseIndex, int AttackIndex)
{
	if (PoseIndex != -1 && AttackIndex != -1)
	{
		FString AttackInfoStr = FString::FromInt(PoseIndex).Append(".").Append(FString::FromInt(AttackIndex));
		FName AttackInfoName = FName(*AttackInfoStr);

		TArray<int>* Bones;
		Bones = PoseAttackHitBoxMap.Find(AttackInfoName);
		if (Bones)
		{
			return *Bones;
		}

		FPoseAttackInfo* AttackInfo = GetPoseAttackInfo(PoseIndex, AttackIndex);
		if (AttackInfo)
		{
			TArray<int> NexBones;
			for (int i = 0; i < AttackInfo->Bone.Num(); ++i)
			{
				int index = -1;
				if (AttackCharacter->HitBoxNames.Find(AttackInfo->Bone[i], index))
				{
					NexBones.Add(index);
				}
			}
			PoseAttackHitBoxMap.Add(AttackInfoName, NexBones);
			return NexBones;
		}
	}
	return TArray<int>();
}

void UCombatSystemComponent::OnPoseMontageEnded(UAnimMontage* poseMtg, bool bInterrupted)
{
	if (bInterrupted)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Interrupted")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ToEnd")));
		
		int CurrentPoseIndex = CurrentActionState->GetPoseIndex();
		MOVE_TYPE currentType = CurrentActionState->GetMoveType();
		
		FPoseStateInfo* PoseInfo = InputCommandCP->GetPoseStateInfo(CurrentPoseIndex);
		
		if (PoseInfo)
		{
			if (PoseInfo->Link != -1)
			{
				FAttackState* CombatAttack = ActoinStateFactory->CreateAttackState((AAttackCharacter*)GetOwner());
				CombatAttack->Init(PoseInfo->Link, ATTACK_ANIM_STATE::ANIM_LINK);
				ChangeActionState(CombatAttack);
			}
			else
			{
				FIdleState* idleState = ActoinStateFactory->CreateIdleState((AAttackCharacter*)GetOwner());
				ChangeActionState(idleState);
			}
		}
	}
}
