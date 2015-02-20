#include "ftpupload.h"

FTPUploader::FTPUploader()
{
	connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(uploadFinished(QNetworkReply*)));
	//connect(&manager, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(erroInterno(QNetworkReply::NetworkError)));
}

void FTPUploader::DoUpload()
{
//	QUrl url("ftp://129.105.36.214/home/jianshao/Depth_Test.avi");
	QByteArray remoteUrl = "ftp://";
	remoteUrl += m_FtpCfg.host;
	remoteUrl += "/";
	remoteUrl += m_FtpCfg.path;
	remoteUrl += "/";
	remoteUrl += m_remoteFileName;

	qDebug()<<"Remote Path:"<<remoteUrl;

	QUrl url(remoteUrl);

//	url.setUserName("jianshao");
//	url.setPassword("111111");
//	url.setPort(21);
	url.setUserName(m_FtpCfg.user);
	url.setPassword(m_FtpCfg.pwd);
	url.setPort(21);

	data = new QFile(m_localPath,this);

	if (data->open(QIODevice::ReadOnly)) {
		qDebug()<<"Start Upload" + m_localPath;
		reply = manager.put(QNetworkRequest(url), data);
		connect(reply, SIGNAL(uploadProgress(qint64, qint64)), SLOT(uploadProgress(qint64, qint64)));
	}
	else
		qDebug() << "Cannot Upload "+m_localPath;
}

void FTPUploader::uploadFinished(QNetworkReply *reply)
{
	qDebug() << "Finished" << reply->error();

	data->deleteLater();
	reply->deleteLater();
}

void FTPUploader::execute()
{
	DoUpload();
}

void FTPUploader::uploadProgress(qint64 bytesSent, qint64 bytesTotal) {
	qDebug() << "Uploaded " << bytesSent << "of " << bytesTotal;
}
