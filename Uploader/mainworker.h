#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QObject>
#include <QRunnable>
#include <QProcess>
#include <QStringList>
#include <QDataStream>
#include "config.h"
#include <curl/curl.h>

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
};

class MainWorker : public QObject, public QRunnable
{
	Q_OBJECT
	Q_ENUMS(QProcess::ExitStatus)
	Q_ENUMS(QProcess::ProcessError)

public:
	explicit MainWorker(QObject* parent=0);
	virtual ~MainWorker(){}

	void SetVideoConverter(const char* program);
	void SetWorkingDirectory(const char* path);
	void SetContext(void* ctx){m_ctx=ctx;}

	void SetFTPConfig(const FTPConfig & cfg)
	{
		m_FTPConfig.host = cfg.host;
		m_FTPConfig.path = cfg.path;
		m_FTPConfig.pwd = cfg.pwd;
		m_FTPConfig.user = cfg.user;
	}

protected:
	void run();

protected slots:
	void OnFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void OnStarted();
	void OnError(QProcess::ProcessError error);

signals:
	void TransformSignal(QString);

private:
	QString m_Converter;
	QString m_WorkDir;
	void* m_ctx;
	bool m_ok;

	FTPConfig m_FTPConfig;
};

#endif // MAINWORKER_H
