// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Meteor/Common/MeteorDef.h"
#include "FPoseInputTable.h"

#include "InputCommamdComponent.generated.h"

using namespace Meteor;
/** 
* 计划按照 如下的思路实现一版
* http://forums.shoryuken.com/discussion/181469/programing-fighting-game-inputs
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEOR_API UInputCommamdComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInputCommamdComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitActionList();

	void GetKeyMapping();

	void TickInputState();

	void TickStateRecord();

	void RecreateStateRecord(int currStateId);

	FPoseStateInfo* GetPoseStateInfo(int stateNo);

	bool CanAttack(ActionStateDef* Action);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputCommand)
	TAssetPtr<UDataTable> Dao_PoseStateInfo;

	// 按键犹豫期
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputCommand)
	int InputHaltTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MoveState)
	int StateNo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MoveState)
	bool bHasControl;

	int LastStateNo;

	STATE_TYPE CurrentStateType;

private:
	int KeyCount;

	TArray<ActionStateDef> ActionList;

	TMap<Meteor::INPUT_KEY, FKey> KeyMapping;

	TArray<bool> CurrentFrameKeyState;

	TArray<Meteor::INPUT_EVENT> CurrentFrameInputEvent;

	TArray<ActionStateRecord> CurrentStateRecord;


};
