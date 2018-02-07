// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

#include "MeteorAnimMetaData.h"
#include "Common/MeteorDef.h"
#include "Common/MeteorActionState.h"
#include "Common/MeteorActionFactory.h"
#include "MeteorDataTable.h"

#include "CombatSystemComponent.generated.h"

using namespace Meteor;

class AAttackCharacter;
class UInputCommandComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEOR_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	friend class AAttackCharacter;

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

	UFUNCTION(BlueprintCallable, Category = "Combat")
	MOVE_TYPE GetMoveType() const;

	UAnimMontage* GetPoseMontage(int PoseIndex);

	FPoseAttackInfo* GetPoseAttackInfo(int PoseIndex, int AttackIndex);

	TArray<int> GetPoseAttackBones(int PoseIndex, int AttackIndex);

	void OnPoseMontageEnded(UAnimMontage* poseMtg, bool bInterrupted);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UDataTable* PoseMontageDB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UDataTable* PoseAttackInfoDB;

	float FrameLinkTime;

	FMeteorActionStateFactory* ActoinStateFactory;
private:
	UInputCommandComponent* InputCommandCP;

	AAttackCharacter* AttackCharacter;

	ATTACK_ANIM_STATE AttackAnimState;

	FActionState* CurrentActionState;

	TMap<FName, TArray<int>> PoseAttackHitBoxMap;
};
