
#pragma once

#include "CoreMinimal.h"
#include "Common//MeteorActionState.h"

class AAttackCharacter;

template<typename  ActionState>
class METEOR_API FMeteorActionPool
{
public:
	FMeteorActionPool();
	FMeteorActionPool(int32 maxSize);
	~FMeteorActionPool();
	ActionState* CreateAction(AAttackCharacter* Owner);
	
private:
	int32 MaxSize;
	int32 Size;
	TArray<ActionState*> ActionPool;
};

template<typename  ActionState>
FMeteorActionPool<ActionState>::FMeteorActionPool()
{
	MaxSize = 4;
	Size = 0;
}

template<typename  ActionState>
FMeteorActionPool<ActionState>::FMeteorActionPool(int32 maxSize)
{
	MaxSize = maxSize;
	Size = 0;
}

template<typename  ActionState>
FMeteorActionPool<ActionState>::~FMeteorActionPool()
{
	int CurrentSize = Size < MaxSize ? Size : MaxSize;
	for (int i = 0; i < CurrentSize; ++i)
	{
		delete ActionPool[i];
		ActionPool[i] = nullptr;
	}
	ActionPool.Empty();
}

template<typename  ActionState>
ActionState* FMeteorActionPool<ActionState>::CreateAction(AAttackCharacter* Owner)
{
	Size++;
	if (Size <= MaxSize)
	{
		ActionState* Action = new ActionState(Owner);
		ActionPool.Add(Action);
		return Action;
	}
	else
	{
		int32 CurrentIndex = (Size - 1) % MaxSize;
		ActionState* Action = ActionPool[CurrentIndex];
		Action->Clear();
		return Action;
	}
}

class METEOR_API FMeteorActionStateFactory
{
public:
	FMeteorActionStateFactory();

	FIdleState* CreateIdleState(AAttackCharacter* Owner);
	FAttackState* CreateAttackState(AAttackCharacter* Owner);
	FGuardState* CreateGuardState(AAttackCharacter* Owner);
	FJumpState* CreateJumpState(AAttackCharacter* Owner);

private:
	FMeteorActionPool<FIdleState> IdlePool;
	FMeteorActionPool<FAttackState> AttackPool;
	FMeteorActionPool<FGuardState> GuardPool;
	FMeteorActionPool<FJumpState> JumpPool;
};
