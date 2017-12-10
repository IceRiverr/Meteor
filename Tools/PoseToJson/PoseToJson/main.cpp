
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

	QString filePath = "D:\\Projects\\Meteor\\Tools\\PoseToJson\\PoseToJson\\PoseFile\\P0\\";
	QString posePath = filePath + "P0.POS";
	QString jsonPath = filePath + "P0.POS.json";

	QString correctJsonPath = filePath + "P0.POS.Correct.json";
	QString baseFrameJsonPath = filePath + "P0.POS.FrameStart.json";
	QString attackCSVPath = filePath + "P0.POS.Attack.csv";

	QString npcExportSection = filePath + "P0.Pose.Export.txt";
	QString characterExportSection = filePath + "P0.Character.Export.txt";

	if (bParse)
	{
		parse_meteor_pose(posePath, jsonPath);
		qDebug() << "Parse initial file succeed!" << endl;
	}
	
	if (bDecStartFrame)
	{
		remove_start_frame(
			jsonPath, 
			correctJsonPath, 
			baseFrameJsonPath, 
			attackCSVPath,
			npcExportSection,
			characterExportSection);
	}

	//getchar();

	return a.exec();
}
