// Copyright IceRiver. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

namespace Meteor
{
	enum class INPUT_EVENT
	{
		INPUT_Pressed = 0,	// ���� �¼�
		INPUT_Released,		// �ͷ� �¼�
		INPUT_Hold,			// �������� �¼�
		INPUT_Idle,			// ���̴��� δ����״̬
	};

	enum class INPUT_KEY
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

	// ������ɫ�ܻ�����
	enum class STATE_TYPE
	{
		TYPE_STANDING,	// S
		TYPE_CROUCHING,	// C
		TYPE_AIR,		// A
		TYPE_LYE_DOWN,	// L
		TYPE_UNCHANGED,	// U
		TYPE_UNKNOWN,
	};

	// ��ǰ����������
	enum class MOVE_TYPE
	{
		MOVE_ATTACK,	// A
		MOVE_IDLE,		// I
		MOVE_BEING_HIT,	// H
		MOVE_UNCHANGED,	// U
	};

	struct InputState
	{
		INPUT_KEY Key;
		INPUT_EVENT Event;
		float HoldTime;
	};

	INPUT_KEY GetInputKey(const TCHAR& c);

	InputState CompileInputState(const FString& cmd, bool& bSucceed);

	FName GetKeyMappingName(const INPUT_KEY& key);
	
	// ����ָ����ѭMUGEN��ָ��������Ҫ��չָ��Ҳ�ô���
	struct InputCommand
	{
		bool CreateInputCommand(FName n, const FString& inputCmd); // ����inputCmd������Command

		FName Name;
		TArray<InputState> Command;
		float Time; // ���������Ҫ��ʱ��
	};

	// ��������ĳ����������
	struct ActionStateDef
	{
		FName name;
		InputCommand cmd; // ������ÿһ��ActionState����Cmd
		STATE_TYPE type;
		MOVE_TYPE moveType;
		int poseNo;
		int juggling;
		float attack;
		int priority;
	};

	class ActionStateRecord
	{
	public:
		bool operator < (const ActionStateRecord& other) const;

		ActionStateDef* Action;
		int CmdState;
		int Timer;
	};
}
