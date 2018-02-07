// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include "Array.h"
#include "AttackKeyClickInfo.h"
#include "Map.h"

class METEOR_API MeteorPose
{
public:
	void Reset();

public:
	int PoseId;
	std::string PoseName;
	TArray<int> NextPoses;
	TArray<Meteor::EAttackKey> AttackKeys;
};

class METEOR_API MeteorSkill
{
public:
	MeteorSkill();
	~MeteorSkill();

private:
	TMap<int, MeteorPose> SkillChangeTable;
};
