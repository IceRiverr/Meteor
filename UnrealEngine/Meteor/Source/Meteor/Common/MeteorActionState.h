
#pragma once

#include "Meteor/Common/MeteorDef.h"

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

	virtual Meteor::MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_IDLE; }

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

	virtual Meteor::MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_IDLE; }
};

class METEOR_API FAttackState : public FActionState
{
public:
	FAttackState(AAttackCharacter* owner);

	void Init(int32 Pose, ATTACK_ANIM_STATE AnimState, float TransitTIme);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual Meteor::MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_ATTACK; }
	
private:
	int32 PoseIndex;
	UAnimMontage* PoseMtg;
	UAnimMetaData_SectionInfo* PoseSectionInfo;

	ATTACK_ANIM_STATE EnterAnimState;
	float AnimTransitTime;
	float AnimTransitTimer;
};

class METEOR_API FDashMoveState : public FActionState
{
public:
	FDashMoveState(AAttackCharacter* owner);

	void Init(int32 Pose, Meteor::DASH_TYPE dash);

	virtual void Enter() override;

	virtual void Execute(float DeltaTime) override;

	virtual void Exit() override;

	virtual void Clear() override;

	virtual Meteor::MOVE_TYPE GetMoveType() const { return MOVE_TYPE::MOVE_DASH; }

private:
	int32 PoseIndex;
	Meteor::DASH_TYPE DashType;
	UAnimMontage* DashMtg;
};
