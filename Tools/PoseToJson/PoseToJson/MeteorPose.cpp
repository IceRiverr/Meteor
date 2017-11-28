#include "MeteorPose.h"
#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qfile.h>
#include <qjsondocument.h>
#include "MeteorPose.h"
#include "QJsonObject"

QString remove_comment(const QString& src, QString comment)
{
	int pos = src.indexOf(comment);
	QString dest;
	if (src >= 0)
		dest = src.left(pos);
	else
		dest = src;
	return dest;
}

QString first_letter_upper(const QString& src)
{
	if (src.length() > 0)
	{
		QChar firtLetter = src.at(0);
		if (firtLetter.isLower())
		{
			QChar up = firtLetter.toUpper();
			QString dest = src;
			dest.replace(0, 1, QString(up));
			return dest;
		}
	}
	return src;
}

bool get_next_line(QFile& file, QString& str)
{
	if (!file.atEnd())
	{
		QByteArray line = file.readLine();
		str = QString(line);
		str = remove_comment(str, "#");
		str = str.simplified();
		return true;
	}
	return false;
}

bool check_is_pose_header(const QString& src, QFile& file, KeyValuePair& pair)
{
	static QRegExp poseHeaderReg("\\bPose\\s+(\\d+).*");
	if (src.indexOf(poseHeaderReg) == 0)
	{
		QString nextStr;
		if (get_next_line(file, nextStr))
		{
			if (nextStr == "{")
			{
				pair = KeyValuePair(VALUE_OBJ, "Pose", poseHeaderReg.cap(1));
				return true;
			}
		}
	}
	return false;
}

bool check_is_object(const QString& src, QFile& file, KeyValuePair& pair)
{
	static QRegExp testReg("\\w+");
	if (testReg.exactMatch(src))
	{
		QString nextStr;
		get_next_line(file, nextStr);

		if (nextStr == "{")
		{
			pair = KeyValuePair(VALUE_OBJ, src, "");
			return true;
		}
	}
	return false;
}

bool check_is_object_end(const QString& src, QFile& file)
{
	if (src == "}")
	{
		return true;
	}
	return false;
}

struct PoseContext
{
	QJsonArray actionArr;
	QJsonArray attackArr;
	QJsonArray dragArr;
};

QJsonObject* pCurrentJson = nullptr;

// 后续遍历
void convert_tree_to_json(Node* node, QJsonObject* jsonObj, PoseContext& context)
{
	if (node == nullptr)
		return;

	QJsonObject obj;
	PoseContext con;

	if (node->children.length() > 0)
	{
		for (int i = 0; i < node->children.length(); ++i)
		{
			convert_tree_to_json(node->children.at(i), &obj, context);
		}
	}

	if (node->pair.type == VALUE_OBJ)
	{
		if (QString::compare(node->pair.key, "action", Qt::CaseInsensitive) == 0 || 
			QString::compare(node->pair.key, "blend", Qt::CaseInsensitive) == 0)
		{
			context.actionArr.push_back(obj);
		}
		else if (QString::compare(node->pair.key, "attack", Qt::CaseInsensitive) == 0)
		{
			context.attackArr.push_back(obj);
		}
		else if (QString::compare(node->pair.key, "drag", Qt::CaseInsensitive) == 0)
		{
			context.dragArr.push_back(obj);
		}
		else if(QString::compare(node->pair.key, "pose", Qt::CaseInsensitive) == 0)
		{
			jsonObj->insert(node->pair.key, obj);
			//jsonObj->insert(node->pair.key, node->pair.value);
		}
		else
		{
			jsonObj->insert(node->pair.key, obj);
		}
	}
	else
	{
		jsonObj->insert(node->pair.key, node->pair.value);
	}
}

bool check_is_key_value(const QString& src, QFile& file, KeyValuePair& keyValue)
{
	static QRegExp keyIntReg("(\\w+)\\s+([0-9]+)$");
	static QRegExp keyFloatReg("(\\w+)\\s+([0-9]+\\.[0-9]+)$");
	static QRegExp colorValueReg("(\\w+)\\s+(\\d+\\,\\d+\\,\\d+).*");
	static QRegExp boneValueReg("\\bbone\\s*=\\s*");
	static QRegExp EffectIDReg("(EffectID)\\s+(\\w+)");

	if (src.indexOf(keyIntReg) >= 0)
	{
		keyValue = KeyValuePair(VALUE_INT, first_letter_upper(keyIntReg.cap(1)), keyIntReg.cap(2));
		return true;
	}

	if (src.indexOf(keyFloatReg) >= 0)
	{
		keyValue = KeyValuePair(VALUE_FLOAT, first_letter_upper(keyFloatReg.cap(1)), keyFloatReg.cap(2));
		return true;
	}

	if (src.indexOf(EffectIDReg) >= 0)
	{
		keyValue = KeyValuePair(VALUE_STR, first_letter_upper(EffectIDReg.cap(1)), EffectIDReg.cap(2));
		return true;
	}

	if (src.indexOf(colorValueReg) >= 0)
	{
		keyValue = KeyValuePair(VALUE_COLOR, first_letter_upper(colorValueReg.cap(1)), colorValueReg.cap(2));
		return true;
	}

	if (src.indexOf(boneValueReg) >= 0)
	{
		QString currentLine = src;
		QString boneStr;
		
		while (currentLine.lastIndexOf(",") == currentLine.length() - 1)
		{
			boneStr.append(currentLine);
			get_next_line(file, currentLine);
		}
		boneStr.append(currentLine);
		
		QStringList bonePair = boneStr.split('=');

		if (bonePair.length() == 2)
		{
			QString boneNames = bonePair[1];
			boneNames.replace(QRegExp("\""), "");
			boneNames = boneNames.simplified();

			keyValue = KeyValuePair(VALUE_BONE, first_letter_upper(bonePair[0]), boneNames);
			return true;
		}
	}
	return false;
}

void write_json_array(const QJsonArray& array)
{
	QJsonDocument jsonDoc(array);
	QByteArray jsonByte = jsonDoc.toJson();
	QFile file("P0_Test.json");
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Open file failed";
		return;
	}

	file.write(jsonByte);
	file.close();
}

int parse_meteor_pose2(const QString& posePath)
{
	QFile poseFile(posePath);
	if (!poseFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Can't open the file." << endl;
		return -1;
	}

	Node* root = new Node();
	Node* current_parent = root;
	
	while (!poseFile.atEnd())
	{
		KeyValuePair pair;
		QString str;
		get_next_line(poseFile, str);
		
		if (check_is_pose_header(str, poseFile, pair))
		{
			Node* poseNode = new Node();
			poseNode->set_parent(current_parent);
			poseNode->str = str;
			poseNode->pair = pair;
			current_parent = poseNode;

			// 添加一个PoseIndex的子节点
			Node* pairNode = new Node();
			KeyValuePair posePair = KeyValuePair(VALUE_INT, "PoseIndex", pair.value);
			pairNode->set_parent(current_parent);
			pairNode->str = str;
			pairNode->pair = posePair;

			//qDebug() << str;
			continue;
		}

		if (check_is_object(str, poseFile, pair))
		{
			Node* objNode = new Node();
			objNode->set_parent(current_parent);
			objNode->str = str;
			objNode->pair = pair;
			current_parent = objNode;

			//qDebug() << str;
			continue;
		}

		if (check_is_object_end(str, poseFile))
		{
			current_parent = current_parent->parent;
			continue;
		}
		
		if (check_is_key_value(str, poseFile, pair))
		{
			//qDebug() << pair.key << pair.value;

			Node* pairNode = new Node();
			pairNode->set_parent(current_parent);
			pairNode->str = str;
			pairNode->pair = pair;

			continue;
		}
	}

	QJsonArray poseArr;
	for (int i = 0; i < root->children.length(); ++i)
	{
		QJsonObject poseObj;
		PoseContext poseContext;
		Node* node = root->children[i];
		convert_tree_to_json(node, &poseObj, poseContext);
		auto p = poseObj.find(node->pair.key);
		
		//poseObj.insert("PoseIndex", node->pair.value);
		if (!poseContext.actionArr.isEmpty())
		{
			poseObj.insert("Action&Blend", poseContext.actionArr);
		}
		if (!poseContext.attackArr.isEmpty())
		{
			poseObj.insert("Attack", poseContext.attackArr);
		}
		if (!poseContext.dragArr.isEmpty())
		{
			poseObj.insert("Drag", poseContext.dragArr);
		}
		poseArr.push_back(poseObj);
	}

	write_json_array(poseArr);

	Node::destroy(&root);

	root = nullptr;

	return 0;
}

Node::Node()
{
	this->parent = nullptr;
}

Node::~Node()
{
	//destroy(this);
}

void Node::destroy(Node** ppNode)
{
	Node* node = *ppNode;
	if (node == nullptr)
		return;

	for (int i = 0; i < node->children.length(); ++i)
	{
		destroy(&(node->children[i]));
	}
	delete node;
	node = nullptr;
}

void Node::visit(Node* node)
{
	if (node == nullptr)
		return;

	qDebug() << node->pair.key << "----" << node->pair.value;

	for (int i = 0; i < node->children.length(); ++i)
	{
		visit(node->children.at(i));
	}
}

void Node::set_parent(Node* parent)
{
	if (parent != nullptr)
	{
		parent->children.append(this);
		this->parent = parent;
	}
}
