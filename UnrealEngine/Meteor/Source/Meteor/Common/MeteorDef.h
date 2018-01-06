// Copyright IceRiver. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

namespace Meteor
{
	UENUM(BlueprintType)
	enum class METEOR_API INPUT_EVENT
	{
		INPUT_Pressed = 0,	// 按下 事件
		INPUT_Released,		// 释放 事件
		INPUT_Hold,			// 持续按下 事件
		INPUT_Idle,			// 键盘处于 未激活状态
	};

	UENUM(BlueprintType)
	enum class METEOR_API INPUT_KEY
	{
		KEY_UP = 0,			// U
		KEY_DOWN,			// D
		KEY_LEFT,			// L
		KEY_RIGHT,			// R
		KEY_ATTACK,			// A
		KEY_DEFENCE,		// B guard
		KEY_JUMP,			// J
		KEY_OTHER,			// other
	};

	// 决定角色受击动作
	UENUM(BlueprintType)
	enum class METEOR_API STATE_TYPE
	{
		TYPE_STANDING,	// S
		TYPE_CROUCHING,	// C
		TYPE_AIR,		// A
		TYPE_LYE_DOWN,	// L
		TYPE_UNCHANGED,	// U
		TYPE_UNKNOWN,
	};

	// 当前动作的类型
	UENUM(BlueprintType)
	enum class METEOR_API MOVE_TYPE
	{
		MOVE_ATTACK,	// A
		MOVE_IDLE,		// I
		MOVE_BEING_HIT,	// H
		MOVE_UNCHANGED,	// U
	};

	UENUM(BlueprintType)
	enum class METEOR_API ATTACK_ANIM_STATE
	{
		ANIM_BEGIN,		// 动画正在切换到下一个	
		ANIM_PLYING,	// 动画正在播放
		ANIM_PAUSING,	// 动画暂停
	};

	UENUM(BlueprintType)
	enum class METEOR_API HIT_TYPE
	{
		HIT_HIT,	// 击中对手
		HIT_DODGE,	// 攻击被闪避
		HIT_MISS,	// 未击中
		HIT_GUARD,	// 攻击被防御
	};
	
	UENUM(BlueprintType)
	struct METEOR_API InputState
	{
		INPUT_KEY Key;
		INPUT_EVENT Event;
		float HoldTime;
	};

	INPUT_KEY GetInputKey(const TCHAR& c);

	InputState CompileInputState(const FString& cmd, bool& bSucceed);

	FName GetKeyMappingName(const INPUT_KEY& key);
	
	// 输入指令遵循MUGEN的指令，后期如果要扩展指令也好处理
	struct METEOR_API InputCommand
	{
		bool CreateInputCommand(FName n, const FString& inputCmd); // 解析inputCmd来生成Command

		FName Name;
		TArray<InputState> Command;
		float Time; // 完成输入需要的时间
	};

	// 用来代表某个攻击动作
	struct METEOR_API ActionStateDef
	{
		FName name;
		InputCommand cmd; // 并不是每一个ActionState都有Cmd
		STATE_TYPE type;
		MOVE_TYPE moveType;
		int poseNo;
		int juggling;
		float attack;
		int priority;
	};

	class METEOR_API ActionStateRecord
	{
	public:
		bool operator < (const ActionStateRecord& other) const;

		ActionStateDef* Action;
		int CmdState;
		int Timer;
	};
}
