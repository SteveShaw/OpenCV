#include "mainworker.h"
#include <QDir>
#include <QDebug>
#include <zmq.h>
#include <QThread>
#include <QFile>


#define MAX_SIZE 512

MainWorker::MainWorker(QObject *parent)
	:QObject(parent)
	,m_ctx(NULL)
{

}

void MainWorker::SetVideoConverter(const char *program)
{
	m_Converter= program;
}

void MainWorker::SetWorkingDirectory(const char *path)
{
	m_WorkDir = path;
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
	QStringList prevFileList;
	QStringList curFileList;

	QDir qDir;
	qDir.setPath(m_WorkDir);

	QStringList filterList;

	filterList<<"*.m4v";

	QStringList changeList;

	char buf[8];

	QByteArray recordBuffer(MAX_SIZE,0);
	//recordBuffer.setRawData()

	prevFileList.clear();

	zmq_pollitem_t items[] = {{recorder,0,ZMQ_POLLIN,0}};

	QByteArray LOCAL_FILE("E:/Video/Depth.m4v");
	QByteArray UPLOAD_FILE_AS("DepthCurl.mp4");
	QByteArray REMOTE_URL("ftp://129.105.36.214/");
	REMOTE_URL+=UPLOAD_FILE_AS;
	QByteArray RENAME_FILE_TO("RenamedDepthCurl.mp4");

	QByteArray remoteUrl("ftp://");
	remoteUrl+=m_FTPConfig.host;
	remoteUrl+="/";



	curl_global_init(CURL_GLOBAL_ALL);
	/* get a curl handle */
	CURL *curl = curl_easy_init();
	CURLcode res;

	if(curl)
	{
		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHeper::read_callback);

		/* enable uploading */
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	}

	QFile qFile;

	QByteArray up = m_FTPConfig.user.toStdString().c_str();
	up+=":";
	up+=m_FTPConfig.pwd;


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
						qFile.setFileName(qDir.path()+"/"+item_list[3]);
//						qDebug()<<qDir.path()+"/"+item_list[3];
						qFile.open(QIODevice::ReadOnly);
						QByteArray url = remoteUrl;
						url += item_list[3];
						/* specify target */
						curl_easy_setopt(curl,CURLOPT_URL, url.data());
//						qDebug()<<url;
						/* now specify which file to upload */
						curl_easy_setopt(curl, CURLOPT_READDATA, &qFile);
//						qDebug()<<up.data();
						curl_easy_setopt(curl, CURLOPT_USERPWD, up.data());

						/* Set the size of the file to upload (optional).  If you give a *_LARGE
							 option you MUST make sure that the type of the passed-in argument is a
							 curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
							 make sure that to pass in a type 'long' argument. */
//						qDebug()<<qFile.size();
						curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)qFile.size());
						/* Now run off and do what you've been told! */
						res = curl_easy_perform(curl);
						/* Check for errors */
						if(res != CURLE_OK)
						{
							emit TransformSignal(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
						}
						else
						{
							emit TransformSignal(QString("Completed uploading File %1").arg(item_list[3].data()));
						}

						qFile.close();
					}

					if(curl)
					{
						qFile.setFileName(qDir.path()+"/"+item_list[4]);
						qFile.open(QIODevice::ReadOnly);
						QByteArray url = remoteUrl;
						url += item_list[4];
						/* specify target */
						curl_easy_setopt(curl,CURLOPT_URL, url.data());
						/* now specify which file to upload */
						curl_easy_setopt(curl, CURLOPT_READDATA, &qFile);
						curl_easy_setopt(curl, CURLOPT_USERPWD, up.data());

						/* Set the size of the file to upload (optional).  If you give a *_LARGE
							 option you MUST make sure that the type of the passed-in argument is a
							 curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
							 make sure that to pass in a type 'long' argument. */
						curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)qFile.size());
						/* Now run off and do what you've been told! */
						res = curl_easy_perform(curl);
						/* Check for errors */
						if(res != CURLE_OK)
						{
							emit TransformSignal(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
						}
						else
						{
							emit TransformSignal(QString("Completed uploading File %1").arg(item_list[4].data()));
						}

						qFile.close();
					}
				}
			}

		}

		QThread::msleep(300);
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


