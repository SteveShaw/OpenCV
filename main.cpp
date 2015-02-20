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
#include <ftpclient.h>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include "colorimagewriter.h"

void LoadSettings(const char* path, DBConfig& db_cfg, FTPConfig &ftp_cfg)
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
}

void SaveSettings(const char* path, const DBConfig& db_cfg, FTPConfig &ftp_cfg)
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

bool TestFTPConn(FTPConfig& cfg)
{

	FTPClient *ftp = new FTPClient(cfg.host.toStdString().c_str(),
																 cfg.user.toStdString().c_str(),
																 cfg.pwd.toStdString().c_str());

	ftp->LogIn();

	bool is_conn = ftp->IsLogged();

	if(is_conn)
	{
		qDebug()<<"Info: FTP is accessible";
		std::list<std::string> fl;
		QString pathName;
		QString today = QDate::currentDate().toString("yyyy-MM-dd");

		//create directory
		bool found = false;

		if(ftp->GetDirectoryList(cfg.path.toStdString().c_str(),fl))
		{
			std::list<std::string>::iterator iter = fl.begin();
			while(iter!=fl.end())
			{
				pathName = (*iter).c_str();
				if(pathName.contains(today))
				{
					cfg.path = pathName;
					found = true;
					break;
				}
				++iter;
			}
		}

		if(!found)
		{
			//create directory with current date
			QString path = cfg.path;
			path.append("/");
			path.append(today);
			is_conn = ftp->MKDir(path.toStdString().c_str());
			if(is_conn)
			{
				cfg.path = path;
			}
		}
		ftp->Close();
	}
	else
	{
		qDebug()<<"Error: Cannot Access to FTP";
	}

	delete ftp;
	return is_conn;
}



int main(int argc, char *argv[])
{

	QCoreApplication a(argc, argv);

	const char* path = "KinectConfig.ini";
	DBConfig db_cfg;
	FTPConfig ftp_cfg;
	LoadSettings(path,db_cfg,ftp_cfg);


//	bool dbOK = TestDBConn(db_cfg);
//	bool dbOK = true;
	bool ftpOK = TestFTPConn(ftp_cfg);



//	//SaveDBSettings(path,db_cfg);


	if(!ftpOK)
	{
		return -1;
	}


	QScopedPointer<KinectCapture> kc(new KinectCapture);

	if(!kc->Initialize())
	{
		qDebug()<<"Cannot Initialized Kinect";

		return -1;
		//        QScopedPointer<Worker> sp_worker(new Worker(&kc,ctx));
	}


	if(argc>=2)
	{
		kc->SetSaveDirectory(argv[1]);
	}

	if(argc>=3)
	{
		kc->SetFrameCount(QString(argv[2]).toInt());
	}

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

	kc->Release();
	kc.reset();

//	zmq_close(sender);
//	zmq_ctx_term(ctx);

	//    delete wk;
	//    delete kc;

	return a.exec();

}
