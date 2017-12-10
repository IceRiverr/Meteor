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

	// 收集子节点信息
	QJsonObject obj;
	PoseContext con;
	if (node->children.length() > 0)
	{
		for (int i = 0; i < node->children.length(); ++i)
		{
			convert_tree_to_json(node->children.at(i), &obj, con);
		}
	}

	if (node->pair.type == VALUE_OBJ)
	{
		// 如果是需要合并成数组的节点，则添加进 context
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
		else
		{
			// 进行最后的插入
			if (!con.actionArr.isEmpty())
			{
				obj.insert("Action&Blend", con.actionArr);
			}
			if (!con.attackArr.isEmpty())
			{
				obj.insert("Attack", con.attackArr);
			}
			if (!con.dragArr.isEmpty())
			{
				obj.insert("Drag", con.dragArr);
			}

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
			QString bone = first_letter_upper(bonePair[0].simplified());
			QString boneNames = bonePair[1];
			boneNames.replace(QRegExp("\""), "");
			boneNames = boneNames.simplified();

			keyValue = KeyValuePair(VALUE_BONE, bone, boneNames);
			return true;
		}
	}
	return false;
}

void write_json_array(const QJsonArray& array, const QString& jsonPath)
{
	QJsonDocument jsonDoc(array);
	QByteArray jsonByte = jsonDoc.toJson();
	QFile file(jsonPath);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Open file failed";
		return;
	}

	file.write(jsonByte);
	file.close();
}

int parse_meteor_pose(const QString& posePath, const QString& jsonPath)
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

	write_json_array(poseArr, jsonPath);

	Node::destroy(&root);

	root = nullptr;

	return 0;
}


bool parse_jsonObj_to_string(QJsonObject& obj, const QString& key, QString& val)
{
	if (obj.contains(key))
	{
		QJsonValue tmp = obj.take(key);
		if (tmp.isString())
		{
			val = tmp.toString();
			return true;
		}
	}
	return false;
}

bool parse_jsonObj_to_int(QJsonObject& obj, const QString& key, int& val)
{
	QString tmpStr;
	if (parse_jsonObj_to_string(obj, key, tmpStr))
	{
		val = tmpStr.toInt();
		return true;
	}
	return false;
}

bool parse_jsonObj_to_float(QJsonObject& obj, const QString& key, float& val)
{
	QString tmpStr;
	if (parse_jsonObj_to_string(obj, key, tmpStr))
	{
		val = tmpStr.toFloat();
		return true;
	}
	return false;
}

int remove_start_frame(const QString& jsonPath, const QString& correctPath, const QString& removeStartFramePath)
{
	QFile poseFile(jsonPath);
	if (!poseFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Can't open the file." << endl;
		return -1;
	}

	QString jsonVal = poseFile.readAll();
	poseFile.close();

	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(jsonVal.toUtf8(), &parseError);

	if (parseError.error == QJsonParseError::NoError)
	{
		if (doc.isArray())
		{
			QJsonArray arr = doc.array();
			qDebug() << arr.count() << endl;

			QVector<MeteorPoseDefine> PoseDefines;
			for (auto it = arr.begin(); it != arr.end(); ++it)
			{
				if (it->isObject())
				{
					QJsonObject poseObj = it->toObject();
					if (poseObj.contains("Pose"))
					{
						QJsonValue poseVal = poseObj.take("Pose");
						//if (poseVal.isObject())
						{
							MeteorPoseDefine define;
							QJsonObject poseInner = poseVal.toObject();
							define.FromJson(poseInner);
							PoseDefines.append(define);
						}
					}
				}
			}
			qDebug() << PoseDefines.length() << endl;

			// correctPath
			QJsonArray correctPoseArr;
			for (int i = 0; i < PoseDefines.length(); ++i)
			{
				QJsonObject poseObj;
				PoseDefines[i].ToJson(poseObj);
				correctPoseArr.push_back(poseObj);
			}
			write_json_array(correctPoseArr, correctPath);

			// dec first frame
			QJsonArray removeStartFramePoseArr;
			for (int i = 0; i < PoseDefines.length(); ++i)
			{
				QJsonObject poseObj;
				PoseDefines[i].RemoveFirstFrame();
				PoseDefines[i].ToJson(poseObj);
				removeStartFramePoseArr.push_back(poseObj);
			}
			write_json_array(removeStartFramePoseArr, removeStartFramePath);
		}
	}
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

MeteorPoseDefine::MeteorPoseDefine()
{
	this->PoseIndex = -1;
	this->Source = -1;
	this->Start = -1;
	this->End = -1;
	this->LoopStart = -1;
	this->LoopEnd = -1;
	this->EffectType = -1;
	this->EffectID = "";
	this->Link = -1;
}

void MeteorPoseDefine::RemoveFirstFrame()
{
	if (PoseIndex != -1 && Source != -1 && this->Start)
	{
		int firstFrame = this->Start;

		this->Start -= firstFrame;
		this->End -= firstFrame;

		if (this->LoopStart != -1)
		{
			this->LoopStart -= firstFrame;
		}

		if (this->LoopEnd != -1)
		{
			this->LoopEnd -= firstFrame;
		}

		for (int i = 0; i < PoseActions.length(); ++i)
		{
			PoseAction& action = PoseActions[i];
			action.Start -= firstFrame;
			action.End -= firstFrame;
		}

		for (int i = 0; i < PoseAttacks.length(); ++i)
		{
			PoseAttack& attack = PoseAttacks[i];
			attack.Start -= firstFrame;
			attack.End -= firstFrame;
		}

		for (int i = 0; i < PoseDrags.length(); ++i)
		{
			PoseDrag& drag = PoseDrags[i];
			drag.Start -= firstFrame;
			drag.End -= firstFrame;
		}

		if (this->ToNextPose.Start != -1)
		{
			this->ToNextPose.Start -= firstFrame;
			this->ToNextPose.End -= firstFrame;
		}
	}
}

void MeteorPoseDefine::ToJson(QJsonObject& poseObj)
{
	if (this->PoseIndex != -1)
	{
		if (this->PoseIndex != -1)
		{
			poseObj.insert("PoseIndex", this->PoseIndex);
		}
		if (this->Source != -1)
		{
			poseObj.insert("Source", this->Source);
		}
		if (this->Start != -1)
		{
			poseObj.insert("Start", this->Start);
		}
		if (this->End != -1)
		{
			poseObj.insert("End", this->End);
		}
		if (this->LoopStart != -1)
		{
			poseObj.insert("LoopStart", this->LoopStart);
		}
		if (this->LoopEnd != -1)
		{
			poseObj.insert("LoopEnd", this->LoopEnd);
		}
		if (this->EffectType != -1)
		{
			poseObj.insert("EffectType", this->EffectType);
		}
		if (!this->EffectID.isEmpty())
		{
			poseObj.insert("EffectID", this->EffectID);
		}
		if (this->Link != -1)
		{
			poseObj.insert("Link", this->Link);
		}

		if (PoseActions.length() > 0)
		{
			QJsonArray actionArr;
			for (int i = 0; i < PoseActions.length(); ++i)
			{
				const PoseAction& action = PoseActions[i];
				QJsonObject actionObj;

				actionObj.insert("Start", action.Start);
				actionObj.insert("End", action.End);
				actionObj.insert("Speed", QString::number(action.Speed, 'f', 2));

				actionArr.push_back(actionObj);
			}
			poseObj.insert("Action&Blend", actionArr);
		}

		if (PoseAttacks.length() > 0)
		{
			QJsonArray attackArr;
			for (int i = 0; i < PoseAttacks.length(); ++i)
			{
				const PoseAttack& attack = PoseAttacks[i];
				QJsonObject attackObj;
				
				attackObj.insert("Bone", attack.Bone);
				attackObj.insert("Start", attack.Start);
				attackObj.insert("End", attack.End);
				attackObj.insert("AttackType", attack.AttackType);
				attackObj.insert("CheckFriend", attack.CheckFriend);
				attackObj.insert("DefenseValue", QString::number(attack.DefenseValue, 'f', 2));
				attackObj.insert("DefenseMove", attack.DefenseMove);
				attackObj.insert("TargetValue", QString::number(attack.TargetValue, 'f', 2));
				attackObj.insert("TargetMove", attack.TargetMove);
				attackObj.insert("TargetPose", attack.TargetPose);
				attackObj.insert("TargetPoseFront", attack.TargetPoseFront);
				attackObj.insert("TargetPoseBack", attack.TargetPoseBack);
				attackObj.insert("TargetPoseLeft", attack.TargetPoseLeft);
				attackObj.insert("TargetPoseRight", attack.TargetPoseRight);
				
				attackArr.push_back(attackObj);
			}
			poseObj.insert("Attack", attackArr);
		}

		if (PoseDrags.length() > 0)
		{
			QJsonArray dragArr;
			for (int i = 0; i < PoseDrags.length(); ++i)
			{
				const PoseDrag& drag = PoseDrags[i];
				QJsonObject dragObj;

				dragObj.insert("Start", drag.Start);
				dragObj.insert("End", drag.End);
				dragObj.insert("Time", QString::number(drag.Time, 'f', 2));
				dragObj.insert("r", drag.r);
				dragObj.insert("g", drag.g);
				dragObj.insert("b", drag.b);
				dragArr.push_back(dragObj);
			}

			poseObj.insert("Drag", dragArr);
		}

		if (ToNextPose.Start != -1 && ToNextPose.End != -1)
		{
			QJsonObject nextPoseObj;

			nextPoseObj.insert("Start", ToNextPose.Start);
			nextPoseObj.insert("End", ToNextPose.End);
			nextPoseObj.insert("Time", QString::number(ToNextPose.Time, 'f', 2));

			poseObj.insert("NextPose", nextPoseObj);
		}
	}
}

void MeteorPoseDefine::FromJson(QJsonObject& poseObj)
{
	parse_jsonObj_to_int(poseObj, "PoseIndex", this->PoseIndex);
	parse_jsonObj_to_int(poseObj, "Source", this->Source);
	parse_jsonObj_to_int(poseObj, "Start", this->Start);
	parse_jsonObj_to_int(poseObj, "End", this->End);
	parse_jsonObj_to_int(poseObj, "LoopStart", this->LoopStart);
	parse_jsonObj_to_int(poseObj, "LoopEnd", this->LoopEnd);
	parse_jsonObj_to_int(poseObj, "EffectType", this->EffectType);
	parse_jsonObj_to_string(poseObj, "EffectID", this->EffectID);
	parse_jsonObj_to_int(poseObj, "Link", this->Link);

	if (poseObj.contains("Action&Blend"))
	{
		QJsonValue tmp = poseObj.take("Action&Blend");
		if (tmp.isArray())
		{
			QJsonArray tmpArr = tmp.toArray();
			for (auto actionIt = tmpArr.begin(); actionIt != tmpArr.end(); ++actionIt)
			{
				if (actionIt->isObject())
				{
					QJsonObject actionObj = actionIt->toObject();
					MeteorPoseDefine::PoseAction poseAction;

					parse_jsonObj_to_int(actionObj, "Start", poseAction.Start);
					parse_jsonObj_to_int(actionObj, "End", poseAction.End);
					parse_jsonObj_to_float(actionObj, "Speed", poseAction.Speed);

					this->PoseActions.append(poseAction);

					//qDebug() << "Action:\t" << poseAction.Start << poseAction.End << poseAction.Speed;
				}
			}
		}
	}

	if (poseObj.contains("Attack"))
	{
		QJsonValue tmp = poseObj.take("Attack");
		if (tmp.isArray())
		{
			QJsonArray tmpArr = tmp.toArray();
			for (auto attackIt = tmpArr.begin(); attackIt != tmpArr.end(); ++attackIt)
			{
				if (attackIt->isObject())
				{
					QJsonObject attackObj = attackIt->toObject();

					MeteorPoseDefine::PoseAttack poseAttack;

					parse_jsonObj_to_string(attackObj, "Bone", poseAttack.Bone);
					parse_jsonObj_to_int(attackObj, "Start", poseAttack.Start);
					parse_jsonObj_to_int(attackObj, "End", poseAttack.End);

					parse_jsonObj_to_int(attackObj, "AttackType", poseAttack.AttackType);
					parse_jsonObj_to_int(attackObj, "CheckFriend", poseAttack.CheckFriend);
					parse_jsonObj_to_float(attackObj, "DefenseValue", poseAttack.DefenseValue);
					parse_jsonObj_to_int(attackObj, "DefenseMove", poseAttack.DefenseMove);
					parse_jsonObj_to_float(attackObj, "TargetValue", poseAttack.TargetValue);

					parse_jsonObj_to_int(attackObj, "TargetMove", poseAttack.TargetMove);
					parse_jsonObj_to_int(attackObj, "TargetPose", poseAttack.TargetPose);
					parse_jsonObj_to_int(attackObj, "TargetPoseFront", poseAttack.TargetPoseFront);
					parse_jsonObj_to_int(attackObj, "TargetPoseBack", poseAttack.TargetPoseBack);
					parse_jsonObj_to_int(attackObj, "TargetPoseLeft", poseAttack.TargetPoseLeft);
					parse_jsonObj_to_int(attackObj, "TargetPoseRight", poseAttack.TargetPoseRight);

					this->PoseAttacks.append(poseAttack);

					//qDebug() << "Attack:\t" << poseAttack.Start << poseAttack.End;
				}
			}
		}
	}

	if (poseObj.contains("Drag"))
	{
		QJsonValue tmp = poseObj.take("Drag");
		if (tmp.isArray())
		{
			QJsonArray tmpArr = tmp.toArray();
			for (auto tmpIt = tmpArr.begin(); tmpIt != tmpArr.end(); ++tmpIt)
			{
				if (tmpIt->isObject())
				{
					QJsonObject dragObj = tmpIt->toObject();

					MeteorPoseDefine::PoseDrag poseDrag;

					QString color;
					parse_jsonObj_to_int(dragObj, "Start", poseDrag.Start);
					parse_jsonObj_to_int(dragObj, "End", poseDrag.End);
					parse_jsonObj_to_float(dragObj, "Time", poseDrag.Time);
					parse_jsonObj_to_string(dragObj, "Color", color);

					QStringList rgb = color.split(",");
					if (rgb.length() == 3)
					{
						poseDrag.r = rgb[0].toInt();
						poseDrag.g = rgb[1].toInt();
						poseDrag.b = rgb[2].toInt();
					}

					this->PoseDrags.append(poseDrag);

					//qDebug() << "Drag:\t" << poseDrag.Start << poseDrag.End;
				}
			}
		}
	}

	if (poseObj.contains("NextPose"))
	{
		QJsonValue tmp = poseObj.take("NextPose");
		if (tmp.isObject())
		{
			QJsonObject nextPostObj = tmp.toObject();
			MeteorPoseDefine::NextPose nextPose;

			parse_jsonObj_to_int(nextPostObj, "Start", nextPose.Start);
			parse_jsonObj_to_int(nextPostObj, "End", nextPose.End);
			parse_jsonObj_to_float(nextPostObj, "Time", nextPose.Time);

			this->ToNextPose = nextPose;

			//qDebug() << "NextP:\t" << nextPose.Start << nextPose.End;
		}
	}

	//qDebug() << "Pose:\t" << this->Start << this->End << this->PoseIndex << this->Source << endl;
}

void MeteorPoseDefine::ToFriendlyString(QString& startEnd)
{
	QString header = QString("Header\tStart\tEnd\tLStart\tLEnd\n");
	QString pose = QString("%1\t%2\t%3\t%4\t%5\n").arg(this->PoseIndex).arg(this->Start).arg(this->End).arg(this->LoopStart).arg(this->LoopEnd);
	QString actions = "";
	for (int i = 0; i < PoseActions.length(); ++i)
	{
		QString action = QString("Action\t%1\t%2\n").arg(PoseActions[i].Start).arg(PoseActions[i].End);
		actions.append(action);
	}

	QString attacks = "";
}

bool MeteorPoseDefine::ToAttackCSVString(QString& csv)
{
	if (this->Source != -1 && this->PoseIndex != -1 && PoseAttacks.length() > 0)
	{
		for (int i = 0; i < PoseAttacks.length(); ++i)
		{
			QString key = QString("%1.%2").arg(this->PoseIndex).arg(i);
			const PoseAttack& attack = PoseAttacks[i];
			csv += attack.ToCSV(key);
		}
		return true;
	}
	return false;
}

QString MeteorPoseDefine::PoseAttack::ToCSV(const QString& key) const
{
	QString boneString = this->Bone;
	boneString.replace(" ", "_");
	boneString.replace(",", "|");
	
	QString csvStr = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15\n")
		.arg(key)
		.arg(boneString)
		.arg(Start)
		.arg(End)
		.arg(AttackType)
		.arg(CheckFriend)
		.arg(QString::number(DefenseValue, 'f', 2))
		.arg(DefenseMove)
		.arg(QString::number(TargetValue, 'f', 2))
		.arg(TargetMove)
		.arg(TargetPose)
		.arg(TargetPoseFront)
		.arg(TargetPoseBack)
		.arg(TargetPoseLeft)
		.arg(TargetPoseRight);
	return csvStr;
}
