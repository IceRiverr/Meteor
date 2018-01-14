﻿
#include "MeteorActionFactory.h"
#include "AttackCharacter.h"


FMeteorActionStateFactory::FMeteorActionStateFactory()
{
	IdlePool = FMeteorActionPool<FIdleState>(4);
	AttackPool = FMeteorActionPool<FAttackState>(4);
}

FIdleState* FMeteorActionStateFactory::CreateIdleState(AAttackCharacter* Owner)
{
	return IdlePool.CreateAction(Owner);
}

FAttackState* FMeteorActionStateFactory::CreateAttackState(AAttackCharacter* Owner)
{
	return AttackPool.CreateAction(Owner);
}