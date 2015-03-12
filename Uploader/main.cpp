#include "mainwindow.h"
#include <QApplication>
#include "workmanager.h"
#include <QScopedPointer>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include "config.h"
#include "singleapp.h"


void LoadSettings(const char* path, FTPConfig& ftpConfig, AppConfig& appConfig, DBConfig& dbConfig)
{
	QSettings settings(QString(path),QSettings::IniFormat);

	ftpConfig.host = settings.value("FTP/host","localhost").toString();
	ftpConfig.user = settings.value("FTP/user","user").toString();
	ftpConfig.pwd = settings.value("FTP/password","password").toString();
	ftpConfig.path = settings.value("FTP/path","/home/share").toString();

	appConfig.videoPath = settings.value("App/SavePath","d:\\video").toString();
	appConfig.kinectID = settings.value("App/Kinect",1).toInt();
	appConfig.roomID = settings.value("App/Kinect",1).toInt();

	dbConfig.db = settings.value("Database/db","kinectdata").toString();
	dbConfig.tbl = settings.value("Database/tbl","KinectVideo").toString();
	dbConfig.host = settings.value("Database/host","129.105.36.214").toString();
	dbConfig.pwd = settings.value("Database/password","123456").toString();
	dbConfig.user = settings.value("Database/user","root").toString();

}

int main(int argc, char *argv[])
{

	SingleApplication app(argc, argv, "Kinect Uploader");

	if (app.isRunning())
	{
		app.sendMessage("Kinect Uploader is running.");
		return 0;
	}

	const char* path = "KinectConfig.ini";
	AppConfig appCfg;
	FTPConfig ftpCfg;
	DBConfig dbCfg;
	LoadSettings(path,ftpCfg,appCfg, dbCfg);

	//qDebug()<<appCfg.videoPath;



	QScopedPointer<WorkManager> mgr(new WorkManager());
	if(!mgr->SetupWorker(appCfg))
	{
		QMessageBox msgBox;
		msgBox.setText(mgr->ErrorMsg());
		msgBox.exec();
		return app.exit();
	}
	else
	{
		mgr->Prepare(ftpCfg,dbCfg);
		mgr->StartWorker();
	}
	//mgr->StartRecorder();


	return app.exec();
}
