// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

#include "MeteorAnimMetaData.h"
#include "Meteor/Common/MeteorDef.h"
#include "Meteor/Common/MeteorActionState.h"
#include "Meteor/Common/MeteorActionFactory.h"

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

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInputCommandComponent(UInputCommandComponent* Com);

	UAnimMetaData_SectionInfo* GetAnimMetaData(UAnimMontage* montage);

	UAnimInstance* GetAnimInstance();

	EAnimFlag GetAnimFlag(UAnimMontage* montage);

	void ChangeActionState(FActionState* newState);

	Meteor::MOVE_TYPE GetMoveType() const;

	UAnimMontage* GetPoseMontage(int PoseIndex);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UDataTable* PoseMontageDB;
	
	/** 快速前后左右移动时，给一个等待时间，如果攻击键还没有按下，则执行Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float DashHaltTime;

	float DashHaltTimer;

	FMeteorActionStateFactory* ActoinStateFactory;
private:
	UInputCommandComponent* InputCommandCP;

	AAttackCharacter* AttackCharacter;

	ATTACK_ANIM_STATE AttackAnimState;

	FActionState* CurrentActionState;
};
