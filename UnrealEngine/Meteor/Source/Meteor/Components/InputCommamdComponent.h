// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Meteor/Common/MeteorDef.h"

#include "InputCommamdComponent.generated.h"

using namespace Meteor;
/** 
* �ƻ����� ���µ�˼·ʵ��һ��
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

private:
	int KeyCount;

	int KeyWaitTime; // ��һ��15֡��ʱ��

	TArray<ActionStateDef> ActionList;

	TMap<Meteor::INPUT_KEY, FKey> KeyMapping;

	TArray<bool> CurrentFrameKeyState;

	TArray<Meteor::INPUT_EVENT> CurrentFrameInputEvent;

	TArray<ActionStateRecord> CurrentStateRecord;
};
