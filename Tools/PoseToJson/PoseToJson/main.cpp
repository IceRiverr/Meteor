
#include <QtCore/QCoreApplication>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qfile.h>
#include <qjsondocument.h>
#include "MeteorPose.h"

int TEST_RegExp();
int TEST_Json1();
int TEST_Json2();
void TEST_Node();
void TEST_first_letter_Upper();

int TEST_parse_pose();

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//TEST_Json1();
	//TEST_RegExp();
	TEST_parse_pose();
	//TEST_Node();
	//TEST_Json2();
	TEST_first_letter_Upper();
	
	qDebug() << "End";
	//getchar();

	return a.exec();
}

int TEST_Json1()
{
	QJsonObject obj1;
	obj1.insert("name", "lily");

	obj1.insert("age", 24);
	QJsonObject add;
	add.insert("city", "Beijing");
	obj1.insert("addr", add);
	qDebug() << obj1;

	QJsonObject obj2;
	obj2.insert("name", "Eva Green");
	obj2.insert("age", 27);
	QJsonObject add2;
	add2.insert("city", "Paris");
	obj2.insert("addr", add2);
	qDebug() << obj2;

	QJsonArray array;
	array.push_back(obj1);
	array.push_back(obj2);

	obj1.insert("test", 1222);

	QJsonDocument jsonDoc(array);
	QByteArray jsonByte = jsonDoc.toJson();
	QFile file("test.json");
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Open file failed";
		return -1;
	}

	file.write(jsonByte);
	file.close();

	return 0;
}

int TEST_Json2()
{
	QJsonObject obj1;
	obj1.insert("name", "lily");
	obj1.insert("age", 24);
	QJsonObject add;
	add.insert("city", "Beijing");
	obj1.insert("addr", add);
	obj1.insert("name", "kity");
	qDebug() << obj1;

	QJsonObject obj2;
	obj2.insert("name", "Eva Green");
	obj2.insert("age", 27);
	QJsonObject add2;
	add2.insert("city", "Paris");
	obj2.insert("addr", add2);
	qDebug() << obj2;

	QJsonArray array;
	array.push_back(obj1);
	array.push_back(obj2);

	

	QJsonDocument jsonDoc(array);
	QByteArray jsonByte = jsonDoc.toJson();
	QFile file("test2.json");
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Open file failed";
		return -1;
	}

	file.write(jsonByte);
	file.close();

	return 0;
}

int TEST_RegExp()
{
	// 正则表达式
	//Pose 279 #簀淮
	QRegExp poseHeaderReg("Pose\\s+(\\d+).*");
	QRegExp keyValueReg("(\\w+)\\s+([-+]?[0-9]*\\.?[0-9]+).*");
	QRegExp colorValueReg("(\\w+)\\s+(\\d+\\,\\d+\\,\\d+).*");

	QString header1 = "Pose 279 #簀淮";
	QString header2 = "Pose 1";
	QString header3 = "Pose   1   ";
	int pos = header3.indexOf(poseHeaderReg);

	if (pos >= 0)
	{
		qDebug() << poseHeaderReg.captureCount();
		qDebug() << poseHeaderReg.capturedTexts();

		qDebug() << poseHeaderReg.cap(0);
		qDebug() << poseHeaderReg.cap(1);
	}

	QString keyValue1 = "TargetPose       096";
	QString keyValue2 = "DefenseValue     0.3";
	pos = keyValue2.indexOf(keyValueReg);

	if (pos >= 0)
	{
		qDebug() << keyValueReg.cap(0);
		qDebug() << keyValueReg.cap(1);
		qDebug() << keyValueReg.cap(2);
	}

	QString colorValue1 = "Color  255,128,128";
	pos = colorValue1.indexOf(colorValueReg);

	if (pos >= 0)
	{
		qDebug() << colorValueReg.cap(0);
		qDebug() << colorValueReg.cap(1);
		qDebug() << colorValueReg.cap(2);
	}
	
	return 0;
}


int TEST_parse_pose()
{
	QString posePath = "D:\\Projects\\Meteor\\Tools\\PoseToJson\\PoseToJson\\Pose";
	//posePath += "\\P0_Test.POS";
	posePath += "\\P0.POS";
	parse_meteor_pose2(posePath);
	return 0;
}

void TEST_Node()
{
	Node* root = new Node();
	root->str = "Root";

	Node* nodeA = new Node();
	nodeA->str = "A";
	nodeA->set_parent(root);

	Node* nodeB = new Node();
	nodeB->str = "B";
	nodeB->set_parent(root);

	Node* nodeC = new Node();
	nodeC->str = "C";
	nodeC->set_parent(root);

	Node* nodeD = new Node();
	nodeD->str = "D";
	nodeD->set_parent(nodeB);

	Node* nodeE = new Node();
	nodeE->str = "E";
	nodeE->set_parent(nodeB);

	Node* nodeF = new Node();
	nodeF->str = "F";
	nodeF->set_parent(nodeC);

	root->visit(root);
}

void TEST_first_letter_Upper()
{
	QString test = "aaccDD";
	if (test.length() > 0)
	{
		QChar firtLetter = test.at(0);
		if (firtLetter.isLower())
		{
			QChar uu = firtLetter.toUpper();
			test.replace(0, 1, QString(uu));
			qDebug() << uu << test;
		}
	}
}
