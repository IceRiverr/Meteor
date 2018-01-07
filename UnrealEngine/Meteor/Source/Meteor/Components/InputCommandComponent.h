// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Meteor/Common/MeteorDef.h"
#include "Engine/StreamableManager.h"
#include "FPoseInputTable.h"

#include "InputCommandComponent.generated.h"

/** 
* 计划按照 如下的思路实现一版
* http://forums.shoryuken.com/discussion/181469/programing-fighting-game-inputs
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

	void RecreateStateRecord(int currStateId);

	FPoseStateInfo* GetPoseStateInfo(int stateNo);

	FPoseChangeTable* GetNextPoseInfo(int stateNo);

	UAnimMontage* GetPoseMontage(int stateNo);

	bool CanAttack(ActionStateDef* Action);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputCommand)
	UDataTable* Dao_PoseStateInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputCommand)
	UDataTable* Dao_PoseChangeInfo;

	// 按键犹豫期
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputCommand)
	int InputHaltTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MoveState)
	int StateNo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MoveState)
	bool bHasControl;

	int LastStateNo;

	STATE_TYPE CurrentStateType;

	bool bCanAttack;

	bool bBeginChain;

private:
	int KeyCount;

	TArray<ActionStateDef> ActionList;

	TMap<Meteor::INPUT_KEY, FKey> KeyMapping;

	TArray<bool> CurrentFrameKeyState;

	TArray<Meteor::INPUT_EVENT> CurrentFrameInputEvent;

	TArray<ActionStateRecord> CurrentStateRecord;

	FStreamableManager StreamMgr;
};
