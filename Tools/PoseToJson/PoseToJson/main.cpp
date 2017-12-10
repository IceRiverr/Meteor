
#include <QtCore/QCoreApplication>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qfile.h>
#include <qjsondocument.h>
#include "MeteorPose.h"
#include "MyTEST.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	bool bParse = true;
	bool bDecStartFrame = true;

	QString filePath = "D:\\Projects\\Meteor\\Tools\\PoseToJson\\PoseToJson\\PoseFile\\";
	QString posePath = filePath + "P0.POS";
	QString jsonPath = filePath + "P0.POS.json";

	QString correctJsonPath = filePath + "P0.POS.Correct.json";
	QString baseFrameJsonPath = filePath + "P0.POS.FrameStart.json";

	if (bParse)
	{
		parse_meteor_pose(posePath, jsonPath);
	}
	qDebug() << "parse end";
	if (bDecStartFrame)
	{
		remove_start_frame(jsonPath, correctJsonPath, baseFrameJsonPath);
	}
	qDebug() << "End";

	getchar();

	return a.exec();
}
