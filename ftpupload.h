#ifndef FTPUPLOAD_H
#define FTPUPLOAD_H

#include <QObject>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <QCoreApplication>
#include "dbitem.h"



class FTPUploader: public QObject
{
	Q_OBJECT

	QNetworkAccessManager manager;
	QFile *data;
	QNetworkReply *reply;

	FTPConfig m_FtpCfg;

	QString m_localPath;
	QString m_remoteFileName;

public:
	FTPUploader();
	void DoUpload();

	void SetConfig(const FTPConfig &cfg)
	{
		m_FtpCfg.host = cfg.host;
		m_FtpCfg.path = cfg.path;
		m_FtpCfg.pwd = cfg.pwd;
		m_FtpCfg.user = cfg.user;
	}

	void Prepare(const char* local_path, const char* remote_file_name)
	{
		m_localPath = local_path;
		m_remoteFileName = remote_file_name;
	}

public slots:
	void execute();
	void uploadFinished(QNetworkReply *reply);
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
};



#endif // FTPUPLOAD_H

