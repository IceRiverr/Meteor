using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityStandardAssets.CrossPlatformInput;

/**
如果一直按着空格键
一直按着左键
一直按着右键 维持防守动作
由按键层 -- 预期动画层--根据外界条件最终生成 预期动作
                    --轻功
                    --爬墙
                    --重力的影响
                    --落地动作
                    --脚和台阶或者斜面的匹配
                    --变招
 
有效按键
必须按下并弹起
鼠标的动作和按键的动作一起记录
一起来推断需要的操作

PlayerController;
int GetNextAction();

AI
    IDLE
    ATTACK
    DEFENSE
    剧情
    ACTIVE

Action:
    Move
    Action 
        Attack
        防守
        巡逻
    必须考虑自己的同伴和Player的动作

AIController;
具体的Ai
直接生成动作层的顺序+寻路即可完成

由数据来启动动画，并且逐帧做好碰撞检测，特别是爬墙的行为

怎样的按键可以确定为一次有效的按键
     
每个人的招式都可以设定为字典

什么情况下应该情况命令列表

DoubleMove什么情况下应该被打断


关于碰撞检测，是使用刚体碰撞，还是自己进行计算，最好是使用引擎的功能
刚体抖动的问题
刚体如何应对轻功系统

关于锁定摄像机Y轴的问题
1 DoubleMove下需要锁定

应该生成 ActionList 的形式，方便和设计扩展

摄像机将来有机会的话，就改掉，现在的Camera和XiaoMing的执行顺序有问题，导致摄像机抖动
XiaoMing所用的Rotation是上一帧的Camera的数据

可以用一个大的管理器来管理整个场景的全部更新操作，然后就不需要单行代码的执行顺序的问题，可以自己编排执行的顺序
	// 类似这样的结构
	class GameManager
	{
		public Player m_Player;
		public IOManager m_IOMgr;
		public Camera m_Camera;
		public AnimSolver m_AnimSolver;	


		void Start()
		{
			m_Player.ManualStart();
			m_IOMgr.ManualStart();
			m_Camera.ManualStart();
			m_AnimSolver.ManualStart();
		}

		void Update()
		{
			m_IOMgr.ScaneKeys();
			m_Player.Move();
			m_Player.Attack();
			m_Player.PlayAnimation();
			m_Player.Physics();
			m_Camera.CalcAndMove();
		}
	}
 */

public enum KEY_CLICK_STATE
{
	KEY_DOWN,
	KEY_HOLD,
	KEY_UP,
	KEY_IDLE,
}

public class CommondKeyRecord
{
	public enum CONTROL_KEY
	{
		INVALID = -1,
		W = 0,
		A,
		S,
		D,
		COUNT,
	}

	// KeyState
	public float timeStamp;
	private KEY_CLICK_STATE[] KeysState;

	private KEY_CLICK_STATE JumpKeyState;
	private KEY_CLICK_STATE LockKeyState;
	//private 

	// 
	public CommondKeyRecord()
	{
		timeStamp = 0.0f;

		KeysState = new KEY_CLICK_STATE[GetKeyCount()];
		for (int i = 0; i < GetKeyCount(); ++i)
		{
			KeysState[i] = KEY_CLICK_STATE.KEY_IDLE;
		}
	}

	public static int GetKeyCount()
	{
		return (int)CONTROL_KEY.COUNT;
	}

	public static CONTROL_KEY GetControlKey(KeyCode key)
	{
		if (key == KeyCode.A) return CONTROL_KEY.A;
		else if (key == KeyCode.S) return CONTROL_KEY.S;
		else if (key == KeyCode.D) return CONTROL_KEY.D;
		else if (key == KeyCode.W) return CONTROL_KEY.W;
		else return CONTROL_KEY.INVALID;
	}

	public static int GetControlKeyIndex(CONTROL_KEY key)
	{
		if (key != CONTROL_KEY.INVALID && key != CONTROL_KEY.COUNT)
			return (int)key;
		return -1;
	}

	public static int GetControlKeyIndex(KeyCode key)
	{
		CONTROL_KEY controlKey = GetControlKey(key);
		if (controlKey != CONTROL_KEY.INVALID)
		{
			return GetControlKeyIndex(controlKey);
		}
		return -1;
	}

	public void SetKeyState(KeyCode key, KEY_CLICK_STATE state)
	{
		int index = GetControlKeyIndex(key);
		if (index != -1)
			KeysState[index] = state;
	}

	public bool IsKeyDownOrHold(KeyCode key)
	{
		int index = GetControlKeyIndex(key);
		if (index != -1)
		{
			if (KeysState[index] == KEY_CLICK_STATE.KEY_DOWN || KeysState[index] == KEY_CLICK_STATE.KEY_HOLD)
				return true;
		}
		return false;
	}

	public bool IsKeyDown(KeyCode key)
	{
		int index = GetControlKeyIndex(key);
		if (index != -1)
		{
			if (KeysState[index] == KEY_CLICK_STATE.KEY_DOWN)
				return true;
		}
		return false;
	}

	public bool GetDownKey(out KeyCode key)
	{
		if (KeysState[(int)CONTROL_KEY.W] == KEY_CLICK_STATE.KEY_DOWN)
		{
			key = KeyCode.W;
			return true;
		}
		if (KeysState[(int)CONTROL_KEY.S] == KEY_CLICK_STATE.KEY_DOWN)
		{
			key = KeyCode.S;
			return true;
		}
		if (KeysState[(int)CONTROL_KEY.D] == KEY_CLICK_STATE.KEY_DOWN)
		{
			key = KeyCode.D;
			return true;
		}
		if (KeysState[(int)CONTROL_KEY.A] == KEY_CLICK_STATE.KEY_DOWN)
		{
			key = KeyCode.A;
			return true;
		}
		key = KeyCode.A;
		return false;
	}
}

public class NormalMoveCommond
{
	public Vector3 moveSpeed;
}

public class DoubleMoveCommond
{
	public float durationTime;
	public float timeStamp;

	public Vector3 moveSpeed;
}

public class JumpMoveCommond
{
	public float timeStamp;
}

public enum ACTION_STATE
{
	IDLE,
	MOVE_NORMAL,
	MOVE_DOUBLE,
	MOVE_JUMP,
}

public class XiaoMeng : MonoBehaviour
{
	// public
	public Camera m_MainCamera;
	
	// Move Speed
	public float m_MoveForwardSpeed = 2.0f;
	public float m_MoveBackSpeed = 1.0f;
	public float m_MoveLeftSpeed = 1.0f;
	public float m_MoveRightSpeed = 1.0f;
	public float m_DoubleMoveSpeed = 10.0f;

	// Check Time
	private float m_KeyCheckDuration;

	// ACTION_STATE
	private ACTION_STATE m_ActionState;
	private NormalMoveCommond m_NormalMoveCmd;
	private DoubleMoveCommond m_DoubleMoveCmd;
	private JumpMoveCommond m_JumpMoveCmd;

	// KeyRecourd
	private int m_MaxRecordSize;
	private List<CommondKeyRecord> m_KeyRecordList;
	private List<KeyCode> m_ClickKeyList; // 查询顺序位 倒着查 如果出招表位 AAW, 则首先第一个A键出现在该List的末尾;

	// Lock H rotate
	private bool m_LockHView;

	public bool GetIsLockHView()
	{
		return m_LockHView;
	}

	// Function

	void Awake()
	{
		m_KeyCheckDuration = 0.2f;
		m_MaxRecordSize = 200; // 最多记录1000条按键记录 m_KeyCheckDuration * fps = 0.2f * 1000
		m_KeyRecordList = new List<CommondKeyRecord>();
		m_ClickKeyList = new List<KeyCode>();

		//m_MoveForwardSpeed = 2.0f;
		//m_MoveBackSpeed = 1.0f;
		//m_MoveLeftSpeed = 1.0f;
		//m_MoveRightSpeed = 1.0f;
		
		m_ActionState = ACTION_STATE.IDLE;
		m_NormalMoveCmd = new NormalMoveCommond();
		m_DoubleMoveCmd = new DoubleMoveCommond();
		m_DoubleMoveCmd.durationTime = 0.15f;
		m_JumpMoveCmd = new JumpMoveCommond();

		m_LockHView = false;
	}

	void Start()
	{
		
	}

	void Update()
	{
		Quaternion quat = m_MainCamera.transform.rotation;
		Vector3 euler = quat.eulerAngles;
		euler.x = 0.0f;
		euler.z = 0.0f;
		quat = Quaternion.Euler(euler);
		transform.rotation = quat;

		GetInputState();
		ChangeActionState();

		Vector3 posWS = transform.position;
		switch (m_ActionState)
		{
			case ACTION_STATE.IDLE:
				break;
			case ACTION_STATE.MOVE_NORMAL:
				posWS += m_NormalMoveCmd.moveSpeed * Time.deltaTime;
				break;
			case ACTION_STATE.MOVE_DOUBLE:
				posWS += m_DoubleMoveCmd.moveSpeed * Time.deltaTime;
				break;
			default:
				break;
		}

		transform.position = posWS;

		// 处理碰撞检测，在发生碰撞的地方停止
		
		// 在拐角 轻功 战斗锁定 战斗未锁定 的不同视角算法

	}

	static KEY_CLICK_STATE GetKeyClickState(KeyCode key)
	{
		if (Input.GetKey(key))
		{
			if (Input.GetKeyDown(key))
			{
				return KEY_CLICK_STATE.KEY_DOWN;
			}
			else
			{
				return KEY_CLICK_STATE.KEY_HOLD;
			}
		}
		else if (Input.GetKeyUp(key))
		{
			return KEY_CLICK_STATE.KEY_UP;
		}
		else
		{
			return KEY_CLICK_STATE.KEY_IDLE;
		}
	}

	public bool IsKeyDownOrHold()
	{
		if (m_KeyRecordList.Count > 0)
		{
			CommondKeyRecord KeyRecord = m_KeyRecordList[m_KeyRecordList.Count];
			if (KeyRecord.IsKeyDownOrHold(KeyCode.A) ||
				KeyRecord.IsKeyDownOrHold(KeyCode.S) ||
				KeyRecord.IsKeyDownOrHold(KeyCode.D) ||
				KeyRecord.IsKeyDownOrHold(KeyCode.W))
			{
				return true;
			}
		}
		return false;
	}

	bool GetInputState()
	{
		KEY_CLICK_STATE KeyAState = GetKeyClickState(KeyCode.A);
		KEY_CLICK_STATE KeySState = GetKeyClickState(KeyCode.S);
		KEY_CLICK_STATE KeyDState = GetKeyClickState(KeyCode.D);
		KEY_CLICK_STATE KeyWState = GetKeyClickState(KeyCode.W);

		bool bAttackeyActive = (KeyAState != KEY_CLICK_STATE.KEY_IDLE) || 
			(KeySState != KEY_CLICK_STATE.KEY_IDLE) || 
			(KeyDState != KEY_CLICK_STATE.KEY_IDLE) || 
			(KeyWState != KEY_CLICK_STATE.KEY_IDLE);

		if (bAttackeyActive)
		{
			CommondKeyRecord KeyRecord = new CommondKeyRecord();
			KeyRecord.timeStamp = Time.time;
			KeyRecord.SetKeyState(KeyCode.A, KeyAState);
			KeyRecord.SetKeyState(KeyCode.S, KeySState);
			KeyRecord.SetKeyState(KeyCode.D, KeyDState);
			KeyRecord.SetKeyState(KeyCode.W, KeyWState);
			
			while (m_KeyRecordList.Count > m_MaxRecordSize)
			{
				m_KeyRecordList.RemoveAt(0);
			}
			m_KeyRecordList.Add(KeyRecord);

			CalcClickKeys(m_KeyCheckDuration);

			return true;
		}
		return false;
	}

	private void ChangeActionStateV2()
	{
		switch (m_ActionState)
		{
			case ACTION_STATE.IDLE:

				break;
			case ACTION_STATE.MOVE_NORMAL:
				break;

			case ACTION_STATE.MOVE_DOUBLE:
				break;

			case ACTION_STATE.MOVE_JUMP:
				break;
		}
	}

	private void ChangeActionState()
	{
		IsKeysClicked(KeyCode.A);

		ACTION_STATE lastActionState = m_ActionState;
		if (m_ActionState == ACTION_STATE.MOVE_DOUBLE)
		{
			if (Time.time > m_DoubleMoveCmd.timeStamp + m_DoubleMoveCmd.durationTime)
			{
				m_ActionState = ACTION_STATE.IDLE;
				m_LockHView = false;
				m_KeyRecordList.Clear(); // 清空 KeyRecords
				return;
			}
			else
			{
				return;
			}
		}

		if (m_KeyRecordList.Count > 0)
		{
			if (IsKeysClicked(KeyCode.A, KeyCode.W))
				Debug.Log("A-W: BOOM");

			if (lastActionState != ACTION_STATE.MOVE_DOUBLE)
			{
				if (IsKeysClicked(KeyCode.A, KeyCode.A))
				{
					m_ActionState = ACTION_STATE.MOVE_DOUBLE;
					m_DoubleMoveCmd.timeStamp = Time.time;
					m_DoubleMoveCmd.moveSpeed = -transform.right * m_DoubleMoveSpeed;
					m_LockHView = true;
				}
				else if (IsKeysClicked(KeyCode.S, KeyCode.S))
				{
					m_ActionState = ACTION_STATE.MOVE_DOUBLE;
					m_DoubleMoveCmd.timeStamp = Time.time;
					m_DoubleMoveCmd.moveSpeed = -transform.forward * m_DoubleMoveSpeed;
					m_LockHView = true;
				}
				else if (IsKeysClicked(KeyCode.D, KeyCode.D))
				{
					m_ActionState = ACTION_STATE.MOVE_DOUBLE;
					m_DoubleMoveCmd.timeStamp = Time.time;
					m_DoubleMoveCmd.moveSpeed = transform.right * m_DoubleMoveSpeed;
					m_LockHView = true;
				}
				else if (IsKeysClicked(KeyCode.W, KeyCode.W))
				{
					m_ActionState = ACTION_STATE.MOVE_DOUBLE;
					m_DoubleMoveCmd.timeStamp = Time.time;
					m_DoubleMoveCmd.moveSpeed = transform.forward * m_DoubleMoveSpeed;
					m_LockHView = true;
				}
				else
				{
					// 最后处理移动
					CalcMoveCommond();
				}
			}
		}
	}

	private bool CalcMoveCommond()
	{
		if (m_KeyRecordList.Count > 0)
		{
			Vector3 moveSpeed = Vector3.zero;
			CommondKeyRecord KeyRecord = m_KeyRecordList[m_KeyRecordList.Count - 1];
			if(KeyRecord.IsKeyDownOrHold(KeyCode.W))
				moveSpeed += transform.forward * m_MoveForwardSpeed;
			if(KeyRecord.IsKeyDownOrHold(KeyCode.S))
				moveSpeed -= transform.forward * m_MoveBackSpeed;
			if (KeyRecord.IsKeyDownOrHold(KeyCode.A))
				moveSpeed -= transform.right * m_MoveLeftSpeed;
			if (KeyRecord.IsKeyDownOrHold(KeyCode.D))
				moveSpeed += transform.right * m_MoveBackSpeed;

			m_ActionState = ACTION_STATE.MOVE_NORMAL;
			m_NormalMoveCmd.moveSpeed = moveSpeed;
			return true;
		}
		m_ActionState = ACTION_STATE.IDLE;
		return false;
	}

	private void CalcClickKeys(float duration)
	{
		m_ClickKeyList.Clear();
		for (int i = m_KeyRecordList.Count - 1; i >= 0; --i)
		{
			CommondKeyRecord KeyRecord = m_KeyRecordList[i];
			if (Time.time > KeyRecord.timeStamp + duration)
			{
				break;
			}
			KeyCode downKey;
			if (KeyRecord.GetDownKey(out downKey))
			{
				m_ClickKeyList.Add(downKey);
			}
		}
	}

	private bool IsKeysClicked(KeyCode key1)
	{
		if (m_ClickKeyList.Count >= 1)
		{
			if (m_ClickKeyList[m_ClickKeyList.Count - 1] == key1)
			{
				return true;
			}
		}
		return false;
	}

	private bool IsKeysClicked(KeyCode key1, KeyCode key2)
	{
		if (m_ClickKeyList.Count >= 2)
		{
			if (m_ClickKeyList[m_ClickKeyList.Count - 1] == key1 &&
				m_ClickKeyList[m_ClickKeyList.Count - 2] == key2)
			{
				return true;
			} 
		}
		return false;
	}

	private bool IsKeysClicked(KeyCode key1, KeyCode key2, KeyCode key3)
	{
		if (m_ClickKeyList.Count >= 3)
		{
			if (m_ClickKeyList[m_ClickKeyList.Count - 1] == key1 &&
				m_ClickKeyList[m_ClickKeyList.Count - 2] == key2 &&
				m_ClickKeyList[m_ClickKeyList.Count - 3] == key3)
			{
				return true;
			}
		}
		return false;
	}
}
