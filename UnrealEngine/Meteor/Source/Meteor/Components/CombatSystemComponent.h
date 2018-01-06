// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "MyAnimMetaData.h"
#include "Meteor/Common/MeteorDef.h"

#include "CombatSystemComponent.generated.h"

using namespace Meteor;

class AAttackCharacter;
class UInputCommandComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEOR_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatSystemComponent();

	/*enum class SPRINT_DIRECTION
	{
		SPRINT_IDLE = 0,
		SPRINT_FWD,
		SPRINT_BWD,
		SPRINT_RIGHT,
		SPRINT_LEFT,
	};*/

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInputCommandComponent(UInputCommandComponent* Com);

	void UpdateCombatSystem(float DeltaTime);

	void UpdateIdleState(float DeltaTime);

	void UpdateAttackState(float DeltaTime);

	void UpdateDashState(float DeltaTime);

	void ChangeMoveType(MOVE_TYPE newType, MOVE_TYPE lastType);

	void ChangeAttackState(ATTACK_ANIM_STATE newState, ATTACK_ANIM_STATE lastState);

	UAnimMetaData_SectionInfo* GetAnimMetaData(UAnimMontage* montage);

	UAnimInstance* GetAnimInstance();

	EAnimFlag GetAnimFlag(UAnimMontage* montage);

public:
	// 枚举类作为属性会出问题
	MOVE_TYPE MoveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintForwadMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintBackwardMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintRightMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintLeftMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float NextPoseTime;

	/** 快速前后左右移动时，给一个等待时间，如果攻击键还没有按下，则执行Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprint)
	float MaxSprintWaitTime;

private:
	UInputCommandComponent* InputCommandCP;

	AAttackCharacter* AttackCharacter;

	ATTACK_ANIM_STATE AttackAnimState;

	float NextPoseTimer;

	UAnimMontage* NextPoseMtg;

	float SprintHoldTimer;

	EAnimFlag CurrentAnimFlag;
};
