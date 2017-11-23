// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Array.h"

namespace Meteor
{
	enum EAttackKey
	{
		ATTACK_UP = 0,
		ATTACK_DOWN,
		ATTACK_RIGHT,
		ATTACK_LEFT,
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

UENUM(BlueprintType)
namespace ECombooKey
{
	enum Type
	{
		COM_INVALID,
		COM_A,
		COM_U_A,
		COM_D_A,
		COM_L_A,
		COM_R_A,
	};
}