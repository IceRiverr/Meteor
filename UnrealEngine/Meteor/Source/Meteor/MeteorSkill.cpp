// Fill out your copyright notice in the Description page of Project Settings.

#include "MeteorSkill.h"
#include "Meteor.h"
#include <string>

MeteorSkill::MeteorSkill()
{
	MeteorPose pose;

	// 295(296,303)
	pose.Reset();
	pose.PoseId = 295;
	pose.PoseName = "295";
	int nextPoses[] = { 296, 303 };
	pose.NextPoses.Append(nextPoses, sizeof(nextPoses) / sizeof(nextPoses));
	Meteor::EAttackKey attackKeys[] = { Meteor::EAttackKey::ATTACK_UP };
	pose.AttackKeys.Append(attackKeys, sizeof(attackKeys) / sizeof(Meteor::EAttackKey));



}

MeteorSkill::~MeteorSkill()
{
}

void MeteorPose::Reset()
{
	PoseId = -1;
	PoseName = "";
	NextPoses.Empty();
	AttackKeys.Emplace();
}
