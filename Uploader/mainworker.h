#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QObject>
#include <QRunnable>
#include <QProcess>
#include <QStringList>
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <curl/curl.h>
#include "config.h"

class FileListHelper
{
public:
	static void CompareFileList(QStringList& prev, QStringList& cur, QStringList& change)
	{
		if(prev.size()>0)
		{
			foreach(const QString& fileName, cur)
			{
				int pos = prev.indexOf(fileName);

				if(pos<0)
				{
					change.append(fileName);
				}
			}
		}
		else
		{
			change = cur;
		}

		prev = cur;
	}
};

class CurlHeper
{
public:
	static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		curl_off_t nread;
		/* in real-world cases, this would probably get this data differently
			 as this fread() stuff is exactly what the library already would do
			 by default internally */
		QIODevice* io = static_cast<QIODevice*>(stream);
		nread = io->read((char*)ptr,size*nmemb);
//		size_t retcode = fread(ptr, size, nmemb, static_cast<FILE*>(stream));

//		nread = (curl_off_t)retcode;

//		fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
//						" bytes from file\n", nread);
		return nread;
	}

	static size_t append_response(void *ptr, size_t size, size_t nmemb, void *data)
	{
		QList<QByteArray>* pMsgList = (QList<QByteArray>*)data;
		QByteArray msg((char*)ptr,size*nmemb);
		pMsgList->append(msg);

		return msg.size();
//		FILE *writehere = (FILE *)data;
//		return fwrite(ptr, size, nmemb, writehere);
	}
};

class MainWorker : public QObject, public QRunnable
{
	Q_OBJECT
	Q_ENUMS(QProcess::ExitStatus)
	Q_ENUMS(QProcess::ProcessError)

public:
	explicit MainWorker(QObject* parent=0);
	virtual ~MainWorker(){}


	void SetWorkingDirectory(const char* path);
	void SetContext(void* ctx){m_ctx=ctx;}

	void SetFTPConfig(const FTPConfig & cfg)
	{
		m_FTPConfig.host = cfg.host;
		m_FTPConfig.path = cfg.path;
		m_FTPConfig.pwd = cfg.pwd;
		m_FTPConfig.user = cfg.user;
	}

	static CURLcode UploadFile(CURL* curl, QFile &qFile, QDir &qDir, const char* targetDir, const char* file, const char *up);
	static bool CheckDirectoryExists(CURL* curl, const char* host, const char *root, const char *target, const char *up);
	static CURLcode CreateFTPDirectory(CURL *curl, const char *host, const char *root, const char *target, const char *up);
protected:
	void run();

protected slots:
	void OnFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void OnStarted();
	void OnError(QProcess::ProcessError error);

signals:
	void TransformSignal(QString);

private:
	QString m_WorkDir;
	void* m_ctx;
	bool m_ok;

	FTPConfig m_FTPConfig;
};

#endif // MAINWORKER_H
