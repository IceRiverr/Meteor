#pragma once

#include "qstring.h"
#include "qvector.h"

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

int parse_meteor_pose2(const QString& posePath);

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
