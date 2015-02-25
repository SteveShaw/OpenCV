#include "mainworker.h"
//#include <QDir>
#include <QDebug>
#include <zmq.h>
#include <QThread>
//#include <QFile>



#define MAX_SIZE 512

MainWorker::MainWorker(QObject *parent)
	:QObject(parent)
	,m_ctx(NULL)
{

}



void MainWorker::SetWorkingDirectory(const char *path)
{
	m_WorkDir = path;
}

CURLcode MainWorker::UploadFile(CURL *curl, QFile &qFile, QDir& qDir, const char *targetDir, const char *file,
																const char* up)
{
	qFile.setFileName(qDir.path()+"/"+file);

	qFile.open(QIODevice::ReadOnly);
	QByteArray url = targetDir;
	url.append(file);
	/* specify target */
	curl_easy_setopt(curl,CURLOPT_URL, url.data());

	qDebug()<<url;
	/* now specify which file to upload */
	curl_easy_setopt(curl, CURLOPT_READDATA, &qFile);
	//						qDebug()<<up.data();
	curl_easy_setopt(curl, CURLOPT_USERPWD, up);

	/* Set the size of the file to upload (optional).  If you give a *_LARGE
		 option you MUST make sure that the type of the passed-in argument is a
		 curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		 make sure that to pass in a type 'long' argument. */
	//						qDebug()<<qFile.size();
	curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)qFile.size());
	/* Now run off and do what you've been told! */
	CURLcode res = curl_easy_perform(curl);

	qFile.close();
	return res;
}

bool MainWorker::CheckDirectoryExists(CURL *curl, const char *host, const char* root, const char* target, const char* up)
{
	QByteArray url = "ftp://";
	url.append(host);
	if(!url.endsWith("/"))url.append("/");
	url.append(root);
	qDebug()<<url;

	QList<QByteArray> msgList;

	curl_easy_setopt(curl, CURLOPT_USERPWD, up);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlHeper::append_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&msgList);

	CURLcode ret =  curl_easy_perform(curl);

	if(ret==CURLE_OK)
	{
		foreach(const QByteArray& msg, msgList)
		{
			if(msg.contains(target))
			{
				return true;
			}
		}
	}

	return false;
}

CURLcode MainWorker::CreateFTPDirectory(CURL* curl,const char *host, const char* root, const char* target, const char* up)
{
	QByteArray curl_cmd = "MKD ";
	curl_cmd += root;
	if(!curl_cmd.endsWith("/"))curl_cmd+="/";
	curl_cmd.append(target);
	qDebug()<<curl_cmd;
	//			curl_cmd += m_FTPConfig.path;
	//			curl_cmd += "/";
	//			curl_cmd += QDateTime::currentDateTime().toString("yyyy-MM-dd");

	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist,curl_cmd.data());

	QByteArray url = "ftp://";
	url.append(host);
	curl_easy_setopt(curl, CURLOPT_USERPWD, up);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist);

	return curl_easy_perform(curl);
}

void MainWorker::run()
{
	//create message queue
	void* rcv = zmq_socket(m_ctx,ZMQ_SUB);
	zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
	zmq_connect(rcv,"inproc://stopper");

	void* recorder = zmq_socket(m_ctx,ZMQ_SUB);
	zmq_setsockopt(recorder, ZMQ_SUBSCRIBE, 0,0);
	zmq_connect(recorder, "tcp://localhost:7777");




	//do loop job

	QDir qDir;
	qDir.setPath(m_WorkDir);

	//	QStringList filterList;

	//	filterList<<"*.m4v";

	//	QStringList changeList;

	char buf[8];

	QByteArray recordBuffer(MAX_SIZE,0);
	//recordBuffer.setRawData()

	//	prevFileList.clear();

	zmq_pollitem_t items[] = {{recorder,0,ZMQ_POLLIN,0}};

	//QByteArray LOCAL_FILE("E:/Video/Depth.m4v");
	//QByteArray UPLOAD_FILE_AS("DepthCurl.mp4");
	//QByteArray REMOTE_URL("ftp://129.105.36.214/");
	//REMOTE_URL+=UPLOAD_FILE_AS;
	//QByteArray RENAME_FILE_TO("RenamedDepthCurl.mp4");

	QByteArray remoteUrl("ftp://");
	remoteUrl+=m_FTPConfig.host;
	remoteUrl+="/";


	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	curl_global_init(CURL_GLOBAL_ALL);
	/* get a curl handle */
	CURL *curl = curl_easy_init();
	CURLcode res = CURLE_OK;


	QByteArray up = m_FTPConfig.user.toStdString().c_str();
	up+=":";
	up+=m_FTPConfig.pwd;



//	struct curl_slist *headerlist=NULL;

	if(curl)
	{
		//check url exists

		bool ok = MainWorker::CheckDirectoryExists(curl,m_FTPConfig.host.toStdString().c_str(),
																					 m_FTPConfig.path.toStdString().c_str(),
																					 nowDate.toStdString().c_str(),up);

		qDebug()<<"Exists"<<ok;

		if(!ok)
		{
			res = MainWorker::CreateFTPDirectory(curl,m_FTPConfig.host.toStdString().c_str(),
																						 m_FTPConfig.path.toStdString().c_str(),
																						 nowDate.toStdString().c_str(),up);

			if(res != CURLE_OK)
			{
				emit TransformSignal(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
			}
			else
			{
				emit TransformSignal(QString("Completed create directory %1").arg(nowDate));
			}
		}

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHeper::read_callback);

		/* enable uploading */
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	}


	QFile qFile;

	QByteArray target = "ftp://";
	target += m_FTPConfig.host;
	if(!target.endsWith("/"))target.append("/");

	if(res==CURLE_OK)
	{
		target += m_FTPConfig.path;
		if(!target.endsWith("/"))target.append("/");
		target.append(nowDate);
		if(!target.endsWith("/"))target.append("/");
	}
	//QByteArray post_cmd = "rename ";

	int retry = 0;

	while(true)
	{

		if(zmq_recv(rcv,buf,7,ZMQ_DONTWAIT)>0)
		{
			break;
		}

		int rc = zmq_poll(items,1,3000);

		if(rc==-1)
		{
			break;
		}

		if(items[0].revents & ZMQ_POLLIN)
		{
			int rv_len = zmq_recv(recorder,recordBuffer.data(),MAX_SIZE,ZMQ_DONTWAIT);
			if(rv_len > 0 && rv_len < MAX_SIZE - 1)
			{
				recordBuffer[rv_len]='\0';

				QList<QByteArray> item_list = recordBuffer.split(';');

				if(item_list.size()==5)
				{
					emit TransformSignal(item_list[3]);
					emit TransformSignal(item_list[4]);

					if(curl)
					{
						retry  = 0;

						while(retry<5)
						{
							res = MainWorker::UploadFile(curl,qFile,qDir,target.data(),item_list[3].data(),up.data());

							if(res != CURLE_OK)
							{
								emit TransformSignal(QString("curl_easy_perform() failed: %1. Retry: %2\n").arg(curl_easy_strerror(res)).arg(retry));
							}
							else
							{

								emit TransformSignal(QString("Completed uploading File %1").arg(item_list[3].data()));
								break;
							}

							++retry;
						}


						retry = 0;

						while(retry<5)
						{
							res = MainWorker::UploadFile(curl,qFile,qDir,target.data(),item_list[4].data(),up.data());

							if(res != CURLE_OK)
							{
								emit TransformSignal(QString("curl_easy_perform() failed: %1. Retry: %2\n").arg(curl_easy_strerror(res)).arg(retry));
							}
							else
							{

								emit TransformSignal(QString("Completed uploading File %1").arg(item_list[4].data()));
								break;
							}

							++retry;
						}

					}
				}
			}

		}

		QThread::msleep(300);
	}

	if(curl)
	{
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	zmq_close(rcv);
	zmq_close(recorder);
}

void MainWorker::OnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitStatus==QProcess::NormalExit)
	{
		// QString msg =
		emit TransformSignal(QString("Transform is completed:%1").arg(exitCode));
		m_ok = true;
	}
	else
	{
		emit TransformSignal(QString("Transform is failed:"));
	}
}
void MainWorker::OnStarted()
{
	emit TransformSignal(QString("Transform is started"));
}
void MainWorker::OnError(QProcess::ProcessError error)
{
	// QProcess::FailedToStart 0 The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.
	// QProcess::Crashed 1 The process crashed some time after starting successfully.
	// QProcess::Timedout 2 The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.
	// QProcess::WriteError 4 An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.
	// QProcess::ReadError 3 An error occurred when attempting to read from the process. For example, the process may not be running.
	// QProcess::UnknownError 5 An unknown error occurred. This is the default return value of error().
	QString errMsg = "No Error";
	switch(error)
	{
	case QProcess::Crashed:
		errMsg = "Error: Crashed";
		break;
	case QProcess::FailedToStart:
		errMsg = "Error: Failed to Start";
		break;
	case QProcess::ReadError:
		errMsg = "Error: Read Error";
		break;
	case QProcess::WriteError:
		errMsg = "Error: Write Error";
		break;
	default:
		errMsg = "Error: Unknown Error";
		break;
	}
	emit TransformSignal(errMsg);
}


