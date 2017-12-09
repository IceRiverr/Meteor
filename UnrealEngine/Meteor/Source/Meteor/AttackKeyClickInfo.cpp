// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackKeyClickInfo.h"
#include "Meteor.h"
#include "LogMacros.h"

AttackKeyClickInfo::AttackKeyClickInfo()
{
	Clear();
}

AttackKeyClickInfo::~AttackKeyClickInfo()
{
}

void AttackKeyClickInfo::Clear()
{
	bUpKeyClick = false;
	bDownKeyClick = false;
	bLeftKeyClick = false;
	bRightKeyClick = false;

	TimeStamp = 0.0f;
}

bool AttackKeyClickInfo::IsAnyKeyClicked() const
{
	if (bUpKeyClick || bDownKeyClick || bLeftKeyClick || bRightKeyClick)
		return true;
	return false;
}

bool AttackKeyClickInfo::IsKeyClicked(Meteor::EAttackKey key) const
{
	switch (key)
	{
	case Meteor::ATTACK_UP:
		return bUpKeyClick;
	case Meteor::ATTACK_DOWN:
		return bDownKeyClick;
	case Meteor::ATTACK_LEFT:
		return bLeftKeyClick;
	case Meteor::ATTACK_RIGHT:
		return bRightKeyClick;
	default:
		return false;
	}
}

AttackKeyClickArray::AttackKeyClickArray()
{
	numMaxRecords = 8;
}

void AttackKeyClickArray::PushClickInfo(const AttackKeyClickInfo& ClickInfo)
{
	while (ClickArray.Num() >= numMaxRecords)
	{
		ClickArray.RemoveAt(0);
	}

	ClickArray.Add(ClickInfo);
}

bool AttackKeyClickArray::IsKeysDown(float currentTime, float interval, TArray<Meteor::EAttackKey> AttackKeys)
{
	// UP--UP--DOWN
	int recordNum = ClickArray.Num();
	int checkNum = AttackKeys.Num();

	if (recordNum < checkNum)
		return false;

	// 倒着检查每个键
	for (int i = 0; i < checkNum; ++i)
	{
		Meteor::EAttackKey AttackKey = AttackKeys[checkNum - 1 - i];
		const AttackKeyClickInfo& ClickInfo = ClickArray[recordNum - 1 - i];
		bool bCheckOK = currentTime - ClickInfo.TimeStamp < interval && ClickInfo.IsKeyClicked(AttackKey);
		if (bCheckOK == false)
			return false;
	}
	return true;
}
