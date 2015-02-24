#ifndef WRITER_H
#define WRITER_H

#include <QObject>
#include <QRunnable>
#include <QEventLoop>


#include "dbitem.h"

class KinectCapture;
class FTPUploader;


class UploadThread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	UploadThread(QObject *parent,KinectCapture* kc);
	~UploadThread();

	void SetDB(const DBConfig &cfg)
	{
		m_db_cfg.db = cfg.db;
		m_db_cfg.host = cfg.host;
		m_db_cfg.pwd = cfg.pwd;
		m_db_cfg.tbl = cfg.tbl;
		m_db_cfg.user = cfg.user;
	}

	void SetFTP(const FTPConfig &cfg);
	//void InitializeMessageQueue();
	//bool IsStopped();
	//void ReleaseMessageQueue();

	protected:
		void run();

//	bool SetDB(const QString &hostname, const QString &dbname, const QString &user, const QString &pwd);


private:
	KinectCapture *_kc;

//	void* _ctx;

	//QSqlDatabase m_db;

	DBConfig m_db_cfg;
	FTPConfig m_ftp_cfg;

	FTPUploader *m_ftpUploader;

	QEventLoop *m_evtLoop;
	//void* _sock;

	//    char _buf[8];
public slots:
	void upload_start(QString start, QString end, QString dir, QString color_file, QString depth_file);

};

#endif // WRITER_H
