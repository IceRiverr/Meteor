#pragma once

#include "qstring.h"
#include "qvector.h"
#include "QJsonObject"

// 先定义数据结构
// 原始文件转化为数据对象
// 将数据对象再转化为json

// 以前的写法有问题

enum ValueType
{
	VALUE_INT,
	VALUE_FLOAT,
	VALUE_STR,
	VALUE_COLOR,
	VALUE_BONE,
	VALUE_OBJ,
};

struct KeyValuePair
{
	KeyValuePair() {}

	KeyValuePair(ValueType t, QString k, QString v)
	{
		type = t;
		key = k;
		value = v;
	}

	ValueType type;
	QString key;
	QString value;
};

int parse_meteor_pose(const QString& posePath, const QString& jsonPath);

int remove_start_frame(const QString& jsonPath, const QString& correctPath, const QString& removeStartFramePath);

class Node
{
public:
	Node();
	~Node();
	static void destroy(Node** ppNode);
	static void visit(Node* node);

	void set_parent(Node* parent);

public:
	Node* parent;
	QVector<Node*> children;

	QString str;
	KeyValuePair pair;
};

struct ParseContext
{
	QString context;
	int pos;
};

struct PoseAction
{


};


class MeteorPoseDefine
{
public:
	struct PoseAction
	{
		int Start;
		int End;
		float Speed;
	};

	struct PoseAttack
	{
		QString ToCSV(const QString& key) const;

		QString Bone;
		int Start;
		int End;
		int AttackType;
		int CheckFriend;
		float DefenseValue;
		int DefenseMove;
		float TargetValue;
		int TargetMove;
		int TargetPose;
		int TargetPoseFront;
		int TargetPoseBack;
		int TargetPoseLeft;
		int TargetPoseRight;
	};

	struct PoseDrag
	{
		int Start;
		int End;
		float Time;
		int r, g, b;
	};

	struct NextPose
	{
		int Start;
		int End;
		float Time;

		NextPose()
		{
			Start = -1;
			End = -1;
			Time = 0.0f;
		}
	};
public:
	MeteorPoseDefine();

	void RemoveFirstFrame();

	void ToJson(QJsonObject& poseObj);

	void FromJson(QJsonObject& poseObj);

	void ToFriendlyString(QString& startEnd);

	bool ToAttackCSVString(QString& csv);

public:
	int PoseIndex;
	int Source;
	int Start;
	int End;
	int LoopStart;
	int	LoopEnd;
	int EffectType;
	QString EffectID;
	int Link;

	QVector<PoseAction> PoseActions;
	QVector<PoseAttack> PoseAttacks;
	QVector<PoseDrag> PoseDrags;
	NextPose ToNextPose;
};
