// Copyright IceRiver. All Rights Reserved.

#include "InputCommamdComponent.h"
#include "Engine.h"
#include "Common/MeteorSingletonLibrary.h"

// Sets default values for this component's properties
UInputCommamdComponent::UInputCommamdComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	KeyWaitTime = 15;
}


// Called when the game starts
void UInputCommamdComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitActionList();

	RecreateStateRecord(-1);

	KeyCount = (int)Meteor::INPUT_KEY::KEY_OTHER;

	CurrentFrameKeyState.SetNum(KeyCount);
}


// Called every frame
void UInputCommamdComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UMeteorSingleton* Singleton = UMeteorSingletonLibrary::GetMeteorSingleton();
	if (Singleton)
	{
		Singleton->FrameCount++;
		UE_LOG(LogTemp, Warning, TEXT("InputBufferTick: %d"), Singleton->FrameCount);
	}

	TickInputState();


}

void UInputCommamdComponent::InitActionList()
{
	{
		Meteor::ActionStateDef def;
		def.name = "xuan_feng_zhan";
		def.cmd.CreateInputCommand("L-R-A", "L,R,A");
		def.type = STATE_TYPE::TYPE_STANDING;
		def.moveType = MOVE_TYPE::MOVE_ATTACK;
		def.attack = 20;
		def.juggling = 15;
		def.poseNo = 374;
		ActionList.Add(def);	
	}
	{
		Meteor::ActionStateDef def;
		def.name = "sui_an_lie";
		def.cmd.CreateInputCommand("L-A", "L,A");
		def.type = STATE_TYPE::TYPE_STANDING;
		def.moveType = MOVE_TYPE::MOVE_ATTACK;
		def.attack = 20;
		def.juggling = 15;
		def.poseNo = 374;
		ActionList.Add(def);
	}
}

void UInputCommamdComponent::GetKeyMapping()
{
	const APlayerController* PlayerControler = GetWorld()->GetFirstPlayerController();
	if (PlayerControler)
	{
		for (int i = 0; i < KeyCount; ++i)
		{
			FName mappingName = GetKeyMappingName((Meteor::INPUT_KEY)i);
			const TArray<FInputActionKeyMapping>& InputActionMappings = PlayerControler->PlayerInput->GetKeysForAction(mappingName);
			if (InputActionMappings.Num() > 0)
			{
				const FInputActionKeyMapping& Mapping = InputActionMappings[0];
				KeyMapping.Add((Meteor::INPUT_KEY)i, Mapping.Key);
			}
		}
	}
}

void UInputCommamdComponent::TickInputState()
{
	for (int i = 0; i < CurrentFrameKeyState.Num(); ++i)
	{
		CurrentFrameKeyState[i] = false;
	}

	const APlayerController* PlayerControler = GetWorld()->GetFirstPlayerController();
	for (int i = 0; i < KeyCount; ++i)
	{

		FKey* key = KeyMapping.Find((Meteor::INPUT_KEY)i);
		if (key && PlayerControler->IsInputKeyDown(*key))
		{
			CurrentFrameKeyState[i] = true;
		}
	}
}

void UInputCommamdComponent::TickStateRecord()
{
	CurrentFrameKeyState;

	for (int i = 0; i < CurrentStateRecord.Num(); ++i)
	{
		ActionStateRecord& Record = CurrentStateRecord[i];
		INPUT_KEY key = Record.Action->cmd.Command[Record.CmdState].Key;

		// 需要对第一个输入命令进行特殊处理，只要一直按着或者按下都满足条件

		//if()
	}
}

void UInputCommamdComponent::RecreateStateRecord(int currStateId)
{
	// for test
	for (int i = 0; i < ActionList.Num(); ++i)
	{
		ActionStateRecord record;
		record.Action = &ActionList[i];
		record.CmdState = 0;
		record.Timer = 0;
		CurrentStateRecord.Add(record);
	}
}

