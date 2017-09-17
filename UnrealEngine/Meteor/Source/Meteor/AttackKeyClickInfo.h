// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Array.h"

namespace Meteor
{
	enum EAttackKey
	{
		ATTACK_UP,
		ATTACK_DOWN,
		ATTACK_LEFT,
		ATTACK_RIGHT,
	};
}

class METEOR_API AttackKeyClickInfo
{
public:
	AttackKeyClickInfo();
	~AttackKeyClickInfo();

	void Clear();
	bool IsAnyKeyClicked() const;
	bool IsKeyClicked(Meteor::EAttackKey key) const;

public:
	bool bUpKeyClick;
	bool bDownKeyClick;
	bool bLeftKeyClick;
	bool bRightKeyClick;

	float TimeStamp;
};


class METEOR_API AttackKeyClickArray
{
public:
	AttackKeyClickArray();
	void PushClickInfo(const AttackKeyClickInfo& ClickInfo);
	bool IsKeysDown(float currentTime, float interval, TArray<Meteor::EAttackKey> AttackKeys);

private:
	TArray<AttackKeyClickInfo> ClickArray;
	int numMaxRecords;

};