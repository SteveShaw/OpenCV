#include <QCoreApplication>
#include <QDebug>
#include <QThreadPool>
#include <QScopedPointer>
#include "kinectcapture.h"
#include "workerclassmanager.h"
#include "worker.h"
#include <QSettings>
#include <QString>
#include <QDate>
#include "msgqueue.h"
#include "singleapp.h"
//#include <ftpclient.h>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include "colorimagewriter.h"

void LoadSettings(const char* path, DBConfig& db_cfg, FTPConfig &ftp_cfg, QString& savePath)
{
	QSettings app_cfg(QString(path),QSettings::IniFormat);

	//	DBConfig* cfg = new DBConfig();
	db_cfg.host = app_cfg.value("Database/host","localhost").toString();
	db_cfg.user = app_cfg.value("Database/user","user").toString();
	db_cfg.pwd = app_cfg.value("Database/password","password").toString();
	db_cfg.db = app_cfg.value("Database/db","KinectData").toString();
	db_cfg.tbl = app_cfg.value("Database/tbl","KinectRecord").toString();

	ftp_cfg.host = app_cfg.value("FTP/host","localhost").toString();
	ftp_cfg.user = app_cfg.value("FTP/user","user").toString();
	ftp_cfg.pwd = app_cfg.value("FTP/password","password").toString();
	ftp_cfg.path = app_cfg.value("FTP/path","/home/share").toString();

	savePath = app_cfg.value("App/SavePath","d:\\video").toString();
}

void SaveSettings(const char* path, const DBConfig& db_cfg, FTPConfig &ftp_cfg, QString& savePath)
{
	QSettings app_cfg(QString(path),QSettings::IniFormat);

	app_cfg.setValue("Database/host",db_cfg.host);
	app_cfg.setValue("Database/user",db_cfg.user);
	app_cfg.setValue("Database/password",db_cfg.pwd);
	app_cfg.setValue("Database/db",db_cfg.db);
	app_cfg.setValue("Database/tbl",db_cfg.tbl);

	app_cfg.setValue("FTP/host",ftp_cfg.host);
	app_cfg.setValue("FTP/user",ftp_cfg.user);
	app_cfg.setValue("FTP/password",ftp_cfg.pwd);
	app_cfg.setValue("FTP/path",ftp_cfg.path);

	app_cfg.setValue("App/SavePath",savePath);
}

//bool TestDBConn(const DBConfig &db_cfg)
//{

//	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
//	db.setHostName(db_cfg.host);
//	db.setDatabaseName(db_cfg.db);
//	db.setUserName(db_cfg.user);
//	db.setPassword(db_cfg.pwd);

//	bool ok = db.open();

//	if(ok)
//	{
//		qDebug()<<"Info: Database is accessible";

//		db.close();
//	}
//	else
//	{
//		qDebug()<<"Error: Cannot Access to Database";
//	}

//	return ok;
//}





int main(int argc, char *argv[])
{

	SingleApplication a(argc, argv, "Kinect Recorder");

	if (a.isRunning())
	{
		a.sendMessage("message from other instance of Kinect Recorder.");
		return 0;
	}

	const char* path = "KinectConfig.ini";
	DBConfig db_cfg;
	FTPConfig ftp_cfg;
	QString saveDir;
	LoadSettings(path,db_cfg,ftp_cfg,saveDir);

	//SaveSettings(path,db_cfg,ftp_cfg,saveDir);

	//return 0;

	QScopedPointer<KinectCapture> kc(new KinectCapture);

	if(!kc->Initialize())
	{
		qDebug()<<"Cannot Initialized Kinect";

		return -1;
		//        QScopedPointer<Worker> sp_worker(new Worker(&kc,ctx));
	}


	kc->SetSaveDirectory(saveDir.toStdString().c_str());

//	if(argc>=2)
//	{
//		kc->SetSaveDirectory(argv[1]);
//	}

//	if(argc>=3)
//	{
//		kc->SetFrameCount(QString(argv[2]).toInt());
//	}

	MsgQueue mq;
	mq.Prepare();

	WorkerManager mgr;
	mgr.PrepareWorkers(kc.data(),mq.GetContext());
	mgr.LoadConfig(db_cfg, ftp_cfg);

	mgr.StartWorkers();

	getchar();


	mq.SendStopSignal();
//	zmq_send(sender,"STP",3,0);

	QThreadPool::globalInstance()->waitForDone();

	//kc->Release();
	kc.reset();

	return a.exec();

}
