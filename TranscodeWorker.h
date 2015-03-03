#ifndef FTPWORKER_H
#define FTPWORKER_H

#include <QRunnable>
#include <QProcess>
//#include <QEventLoop>


#include "dbitem.h"

class KinectCapture;
//class FTPUploader;

class TranscodeWorker : public QRunnable
{
public:
	TranscodeWorker(KinectCapture* kc, void* ctx);
	~TranscodeWorker();

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

	static void Transcode(QProcess* proc, const QString& input, const QString &output);

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
