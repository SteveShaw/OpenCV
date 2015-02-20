#ifndef FTPWORKER_H
#define FTPWORKER_H

#include <QRunnable>
//#include <QEventLoop>


#include "dbitem.h"

class KinectCapture;
//class FTPUploader;

class FTPWorker : public QRunnable
{
public:
	FTPWorker(KinectCapture* kc, void* ctx);
	~FTPWorker();

	void SetDB(const DBConfig &cfg)
	{
		m_db_cfg.db = cfg.db;
		m_db_cfg.host = cfg.host;
		m_db_cfg.pwd = cfg.pwd;
		m_db_cfg.tbl = cfg.tbl;
		m_db_cfg.user = cfg.user;
	}

	void SetFTP(const FTPConfig &cfg)
	{
		m_ftp_cfg.user = cfg.user;
		m_ftp_cfg.pwd = cfg.pwd;
		m_ftp_cfg.path = cfg.path;
		m_ftp_cfg.host = cfg.host;
	}

protected:
	void run();

	bool InitDB();

	bool InsertRecord(const QList<QByteArray> &items, bool color, bool depth);

private:
	KinectCapture* m_kc;
	void* m_ctx;

	DBConfig m_db_cfg;
	FTPConfig m_ftp_cfg;

	enum Property
	{
		MAX_SIZE = 512
	};
};

#endif // FTPWORKER_H
