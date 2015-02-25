#include "mainwindow.h"
#include <QApplication>
#include "workmanager.h"
#include <QScopedPointer>
#include <QSettings>
#include <QDebug>
#include "config.h"


void LoadSettings(const char* path, FTPConfig& ftpConfig, AppConfig& appConfig)
{
	QSettings settings(QString(path),QSettings::IniFormat);

	ftpConfig.host = settings.value("FTP/host","localhost").toString();
	ftpConfig.user = settings.value("FTP/user","user").toString();
	ftpConfig.pwd = settings.value("FTP/password","password").toString();
	ftpConfig.path = settings.value("FTP/path","/home/share").toString();

	appConfig.videoPath = settings.value("General/SavePath","d:\\video").toString();
}

int main(int argc, char *argv[])
{
	const char* path = "KinectConfig.ini";
	AppConfig appCfg;
	FTPConfig ftpCfg;
	LoadSettings(path,ftpCfg,appCfg);

	qDebug()<<appCfg.videoPath;

	QApplication a(argc, argv);

	QScopedPointer<WorkManager> mgr(new WorkManager());
	mgr->SetupWorker(appCfg,ftpCfg);
	//mgr->StartRecorder();
	mgr->StartWorker();

	return a.exec();
}
