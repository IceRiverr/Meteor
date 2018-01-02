// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "FPoseInputTable.h"
#include "MyAnimMetaData.h"
#include "Components/InputBufferComponent.h"
#include "Meteor/Common/MeteorDef.h"

#include "AttackCharacter.generated.h"

using namespace Meteor;

UCLASS()
class METEOR_API AAttackCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	enum ATTACK_STATE
	{
		ATK_IDLE,
		ATK_PLAYING,
		ATK_NEXTPOSE,
	};

	enum class SPRINT_DIRECTION
	{
		SPRINT_IDLE = 0,
		SPRINT_FWD,
		SPRINT_BWD,
		SPRINT_RIGHT,
		SPRINT_LEFT,
	};

	struct CombatAction
	{
		FName InputCmd;
		int32 PoseIndex;
	};

	AAttackCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void OnJump();

	void StopJump();

	void OnAttack();

	void StopAttack();

	UAnimMontage* GetPoseMontage(int32 pose);

	void GetAnimMetaData(UAnimMontage* montage);

	EAnimFlag GetAnimFlag(UAnimMontage* montage);

	int32 GetNextPose(int32 poseIdx, const TArray<FString>& inputCmds, bool bInAir = false);

	SPRINT_DIRECTION GetReadySprintDirection(const TArray<FString>& InputCmds);

	int32 GenerateAttackCommond(int32 OldAttackCmd);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputBuffer)
	UInputBufferComponent* InputBufferCP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pose)
	TAssetPtr<UDataTable> PoseInfoTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pose)
	TAssetPtr<UDataTable> Dao_PoseChangeTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pose)
	int32 CurrentPoseIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pose)
	float NextPoseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputBuffer)
	bool bAcceptInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputBuffer)
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveFwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveBwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveRightSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintForwadMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintBackwardMtg;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintRightMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintLeftMtg;

	/** 快速前后左右移动时，给一个等待时间，如果攻击键还没有按下，则执行Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprint)
	float MaxSprintWaitTime;

private:
	FStreamableManager StreamMgr;

	// attack montage
	TMap<FName, float> SectionRatioCache;
	FName NextPoseOut;
	FName NextPoseIn;
	
	FAlphaBlend tmpBlend;

	ATTACK_STATE AttackState;

	float NextPoseTimer;
	UAnimMontage* NextPoseMtg;

	TArray<FPoseInputTable*> Dao_AllPoses;

	bool bIsSprinting; 

	EAnimFlag CurrentAnimFlag;

	float SprintHoldTimer;
};
