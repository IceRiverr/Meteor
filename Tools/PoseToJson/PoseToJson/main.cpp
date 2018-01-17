
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
	
	QString gamePModelPath = "D:\\Projects\\Meteor\\Game\\Meteor\\pmodel\\";
	QString filePath = "D:\\Projects\\Meteor\\Tools\\PoseToJson\\PoseToJson\\PoseFile\\";
	int NPCCount = 20;

	bool bNeedCmdParam = true;
	if (bNeedCmdParam)
	{
		if (argc == 4)
		{
			gamePModelPath = QString(argv[1]);
			filePath = QString(argv[2]);
			NPCCount = QString(argv[3]).toInt();
		}
		else
		{
			return 0;
		}
	}

	for (int i = 0; i < NPCCount; ++i)
	{
		int NPCIndex = i;

		QString posePath = gamePModelPath + QString("P%1.POS").arg(NPCIndex);
		QString jsonPath = filePath + QString("P%1.POS.json").arg(NPCIndex);
		QString correctJsonPath = filePath + QString("P%1.POS.Correct.json").arg(NPCIndex);
		QString baseFrameJsonPath = filePath + QString("P%1.POS.RemoveFrameOffset.json").arg(NPCIndex);
		QString attackCSVPath = filePath + QString("P%1.POS.Attack.csv").arg(NPCIndex);
		QString npcExportSection = filePath + QString("P%1.Pose.Export.txt").arg(NPCIndex);
		QString characterExportSection = filePath + QString("P%1.Character.Export.txt").arg(NPCIndex);
		QString friendlyStartEndPath = filePath + QString("P%1.FriendlyStartEnd.txt").arg(NPCIndex);
		QString poseToCSV = filePath + QString("P%1.PoseToCSV.txt").arg(NPCIndex);

		QString tmp = QString("P%1.POS").arg(NPCIndex);
		
		parse_meteor_pose(posePath, jsonPath);
		qDebug() << "Parse" << tmp << "succeed!";

		generate_meteor_data_file(
			jsonPath,
			correctJsonPath,
			baseFrameJsonPath,
			attackCSVPath,
			npcExportSection,
			characterExportSection,
			friendlyStartEndPath,
			poseToCSV);
		qDebug() << "Generate" << tmp << "succeed!\n";
	}
	return a.exec();
}
