// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Common/MeteorDef.h"
#include "Engine/StreamableManager.h"
#include "MeteorDataTable.h"

#include "InputCommandComponent.generated.h"

/** 
* 计划按照 如下的思路实现一版
* http://forums.shoryuken.com/discussion/181469/programing-fighting-game-inputs
* 这个地方处理错了，不应该清除掉Action的运行状况，这样UU走到一半，才能出来UUA，根本用的不是事件等待
* 当前招式可关联的下一招，只是作为一个条件过滤，即在当前招式的情况下，只对这些招式感兴趣，可以在 CanAttack中进行测试
*/

using namespace Meteor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEOR_API UInputCommandComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UInputCommandComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitActionList();

	void GetKeyMapping();

	void TickInputState();

	void PoseprocessInputBuffer();

	void TickStateRecord();

	void CreateStateRecord(int currStateId);

	void RecreateNextPoses(int currStateId);

	FPoseStateInfo* GetPoseStateInfo(int stateNo);

	FPoseChangeTable* GetPoseChangeInfo(int stateNo);

	ActionStateDef* GetActionStateInfo(int stateNo);

	int GetPoseLockFrame(int stateNo);

	bool CanAttack(ActionStateRecord& Record);

	void OnGuard();

	void StopGuard();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputCommand)
	UDataTable* Dao_PoseStateInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputCommand)
	UDataTable* Dao_PoseChangeInfo;

	// 按键犹豫期
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputCommand)
	int InputHaltTime;

	// 非A攻击命令的犹豫期
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputCommand)
	int NonAttackCommandHaltTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MoveState)
	int CurrentStateNo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveState)
	bool bHasControl;

	STATE_TYPE CurrentStateType;

	bool bBeginChain;

	bool bGuardKeyDown;

	bool bJumpKeyDown;

	int32 GuardPoseIndex;

private:
	int KeyCount;

	TArray<ActionStateDef> ActionList;

	TMap<Meteor::INPUT_KEY, FKey> KeyMapping;

	TArray<bool> CurrentFrameKeyState;

	TArray<Meteor::INPUT_EVENT> CurrentFrameInputEvent;

	TArray<ActionStateRecord> CurrentStateRecord;

	TArray<int> CurrentNextPoses;

	// 不同的武器对应不同的组，先做一个测试
	TArray<int32> IdleStateNextPoses;
};
