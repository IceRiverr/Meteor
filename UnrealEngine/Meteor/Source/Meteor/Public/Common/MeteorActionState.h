
#pragma once

#include "MeteorDef.h"

class AAttackCharacter;
class UInputCommandComponent;
class UCombatSystemComponent;
class UAnimMetaData_SectionInfo;

using namespace Meteor;

class METEOR_API FActionState
{
public:
	FActionState(AAttackCharacter* owner);
	
	virtual void Enter() {};

	virtual void Execute(float DeltaTime) {};

	virtual void Exit() {};

	virtual void Clear() {};

	virtual MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_IDLE; }

	virtual int GetPoseIndex() const { return -1; }

protected:
	AAttackCharacter* Owner;
	UInputCommandComponent* InputCommandCP;
	UCombatSystemComponent* CombatSystemCP;
	UAnimInstance* AnimInstance;
};

class METEOR_API FIdleState : public FActionState
{
public:
	FIdleState(AAttackCharacter* owner);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_IDLE; }

	virtual int GetPoseIndex() const { return -1; }
};

class METEOR_API FAttackState : public FActionState
{
public:
	FAttackState(AAttackCharacter* owner);

	void Init(int32 Pose, ATTACK_ANIM_STATE AnimState, float TransitTime = 0.0f);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_ATTACK; }

	virtual int GetPoseIndex() const { return PoseIndex; }

private:
	int32 PoseIndex;
	UAnimMontage* PoseMtg;
	UAnimMetaData_SectionInfo* PoseSectionInfo;
	FAlphaBlend CacheBlendIn;
	int PoseLockFrame;
	bool bLockFrameFlag;

	ATTACK_ANIM_STATE EnterAnimState;
	float AnimTransitTime;
	float AnimTransitTimer;
};

class METEOR_API FGuardState : public FActionState
{
public:
	FGuardState(AAttackCharacter* owner);

	void Init(int32 Pose);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_GUARD; }

	virtual int GetPoseIndex() const { return PoseIndex; }
private:
	int32 PoseIndex;
	UAnimMontage* GuardMtg;
};

class METEOR_API FJumpState : public FActionState
{
public:
	FJumpState(AAttackCharacter* owner);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_JUMP; }

	virtual int GetPoseIndex() const { return -1; }
};
