#include "mainworker.h"
//#include <QDir>
#include <QDebug>
#include <zmq.h>
#include <QThread>
#include <QTextStream>
#include <QProcess>
#include <QxtCore/QxtBasicFileLoggerEngine>
#include <QxtCore/QxtLogger>


//#include <QFile>



#define MAX_SIZE 512

MainWorker::MainWorker(QObject *parent)
	:QObject(parent)
	,m_ctx(NULL)
	,m_DateChanged(false)
	,m_Proc(NULL)
	,m_ProcExitCode(-1)
{

}


void MainWorker::SetWorkingDirectory(const char *path)
{
	m_WorkDir = path;
}

CURLcode MainWorker::UploadFile(CURL *curl, QFile &qFile, const char *targetDir, const char *file,
																const char* up)
{
	qFile.setFileName(file);

	qFile.open(QIODevice::ReadOnly);
	QByteArray url = targetDir;
	url.append(file);
	/* specify target */
	curl_easy_setopt(curl,CURLOPT_URL, url.data());

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

int MainWorker::CheckDirectoryExists(CURL *curl, const char *host, const char* root,
																		 const char* target, const char* up)
{
	QByteArray url = "ftp://";
	url.append(host);
	if(!url.endsWith("/"))url.append("/");
	url.append(root);

	QList<QByteArray> msgList;

	curl_easy_setopt(curl, CURLOPT_USERPWD, up);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlHeper::append_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&msgList);

	CURLcode ret = CURLE_OK;
	QByteArray errMsg;



	int retry = 0;

	while(retry <=5 )
	{
		++retry;

		ret = curl_easy_perform(curl);
		if(ret==CURLE_OK)
		{
			foreach(const QByteArray& msg, msgList)
			{
				if(msg.contains(target))
				{
					return 1;
				}
			}

			return 0;

		}

		errMsg = curl_easy_strerror(ret);

		if(errMsg.contains("connect to server"))
		{
			continue;
		}
		else
		{
			return -1;
		}
	}

	return -2;
}

CURLcode MainWorker::CreateFTPDirectory(CURL* curl,const char *host, const char* root, const char* target, const char* up)
{
	QByteArray curl_cmd = "MKD ";
	curl_cmd += root;
	if(!curl_cmd.endsWith("/"))curl_cmd+="/";
	curl_cmd.append(target);

	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist,curl_cmd.data());
	QByteArray url = "ftp://";
	url.append(host);
	curl_easy_setopt(curl, CURLOPT_USERPWD, up);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist);
	CURLcode ret = curl_easy_perform(curl);
	curl_slist_free_all (headerlist);

	return ret;
}

bool MainWorker::UploadThroughCURL(QProcess* proc, const QString &fileName, const QString &dirName,
																	 const char *up, const QString &host)
{
	QString command = QString("curl.exe -T %1 --ftp-create-dirs ftp://%2@%3/%4/").arg(fileName).arg(up).arg(host).arg(dirName);
	proc->start(command);
	if(proc->waitForStarted())
	{
		proc->waitForReadyRead();
		proc->waitForBytesWritten();
		proc->waitForFinished();

		while(proc->processId()>0)
		{
			proc->waitForFinished();
		}
		proc->closeWriteChannel();
	}

	return true;
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

	char buf[8];

	QByteArray recordBuffer(MAX_SIZE,0);

	zmq_pollitem_t items[] = {{recorder,0,ZMQ_POLLIN,0}};

//	QByteArray remoteUrl("ftp://");
//	remoteUrl+=m_FTPConfig.host;
//	remoteUrl+="/";

	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	CURL *curl = curl_easy_init();
//	CURLcode res = CURLE_OK;


	QByteArray up = m_FTPConfig.user.toStdString().c_str();
	up+=":";
	up+=m_FTPConfig.pwd;

//	if(curl)
//	{
//		//check url exists


//		int ok = MainWorker::CheckDirectoryExists(curl,m_FTPConfig.host.toStdString().c_str(),
//																							m_FTPConfig.path.toStdString().c_str(),
//																							nowDate.toStdString().c_str(),up);
//		if(ok==0)
//		{
//			curl_easy_cleanup(curl);
//			curl = curl_easy_init();
//			res = MainWorker::CreateFTPDirectory(curl,m_FTPConfig.host.toStdString().c_str(),
//																					 m_FTPConfig.path.toStdString().c_str(),
//																					 nowDate.toStdString().c_str(),up);

//			if(res != CURLE_OK)
//			{
//				qxtLog->error(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
//				emit TransformSignal(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
//			}
//			else
//			{
//				qxtLog->info(QString("Completed create directory %1").arg(nowDate));
//				emit TransformSignal(QString("Completed create directory %1").arg(nowDate));
//			}

//		}

//		curl_easy_cleanup(curl);
//		curl = curl_easy_init();

//		/* we want to use our own read function */
//		curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHeper::read_callback);

//		/* enable uploading */
//		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
//	}


//	QFile qFile;

//	QByteArray target = "ftp://";
//	target += m_FTPConfig.host;
//	if(!target.endsWith("/"))target.append("/");

//	if(res==CURLE_OK)
//	{
//		//target += m_FTPConfig.path;
//		//if(!target.endsWith("/"))target.append("/");
//		target.append(nowDate);
//		if(!target.endsWith("/"))target.append("/");
//	}

	QList<QByteArray> errUploadList;

	QProcess *proc = new QProcess();

	m_Proc = proc;

	QStringList dbItems;
	dbItems<<" "<<" "<<" ";

//	bool ok = false;
//	int pct = 0;

	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
	connect(proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(OnFinished(int,QProcess::ExitStatus)));
//	connect(proc,SIGNAL(readyReadStandardOutput()),this,SLOT(OnReadyReadStdOut()));
//	connect(proc,SIGNAL(readyReadStandardError()),this,SLOT(OnReadyReadStdErr()));

//	proc->setProcessChannelMode(QProcess::ForwardedChannels);

	while(true)
	{

		//check date changed and make new directory on FTP
//		if(m_DateChanged && curl)
//		{
//			curl_easy_cleanup(curl);
//			curl = curl_easy_init();

//			if(curl)
//			{
//				nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
//				int ok = MainWorker::CheckDirectoryExists(curl,m_FTPConfig.host.toStdString().c_str(),
//																									m_FTPConfig.path.toStdString().c_str(),
//																									nowDate.toStdString().c_str(),up);


//				if(ok==0)
//				{
//					curl_easy_cleanup(curl);
//					curl = curl_easy_init();
//					res = MainWorker::CreateFTPDirectory(curl,m_FTPConfig.host.toStdString().c_str(),
//																							 m_FTPConfig.path.toStdString().c_str(),
//																							 nowDate.toStdString().c_str(),up);


//					if(res != CURLE_OK)
//					{
//						qxtLog->error(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
//						emit TransformSignal(QString("curl_easy_perform() failed: %1\n").arg(curl_easy_strerror(res)));
//					}
//					else
//					{
//						qxtLog->info(QString("Completed create directory %1").arg(nowDate));
//						emit TransformSignal(QString("Completed create directory %1").arg(nowDate));

//						if(res==CURLE_OK)
//						{
//							target = "ftp://";
//							target += m_FTPConfig.host;
//							if(!target.endsWith("/"))target.append("/");

//							target += m_FTPConfig.path;
//							if(!target.endsWith("/"))target.append("/");
//							target.append(nowDate);
//							if(!target.endsWith("/"))target.append("/");

//							m_DateChanged = false;
//						}
//					}
//				}

//				curl_easy_cleanup(curl);
//				curl = curl_easy_init();
//				/* we want to use our own read function */
//				curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHeper::read_callback);

//				/* enable uploading */
//				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
//			}

//		}

		//end check date changed

		if(zmq_recv(rcv,buf,7,ZMQ_DONTWAIT)>0)
		{
			break;
		}

		//check whether the error list is not empty and then upload one each time

		if(errUploadList.size()>0)
		{
			emit TransformSignal(QString("Reuploading File %1").arg(errUploadList.at(0).data()));
			UploadThroughCURL(proc,errUploadList.at(0).data(),errUploadList.at(1).data(),up.data(),m_FTPConfig.host);
			while(m_ProcExitCode<0)
			{
				proc->waitForFinished();
			}
//			ok = false;
//			foreach(const QByteArray &info, m_CurlMsg)
//			{
//				QByteArrayList info_items = info.split(' ');

//				pct = info_items[0].toInt(&ok);
//				if(pct==100 && ok)
//				{
//					ok = true;
//					break;
//				}
//			}

			if(m_ProcExitCode==0)
			{
				emit TransformSignal(QString("Completed uploading File %1").arg(errUploadList.at(0).data()));
				errUploadList.pop_front();
				errUploadList.pop_front();
			}

			m_ProcExitCode = -1;
//			if(res==CURLE_OK)
//			{
//				emit TransformSignal(QString("Completed uploading File %1").arg(errUploadList.at(0).data()));
//				errUploadList.pop_front();
//				errUploadList.pop_front();
//			}
		}


		//end checking error list

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
					nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

					QDir localDir;
					localDir.setPath(item_list[2]);

					emit TransformSignal(localDir.absoluteFilePath(item_list[3]));
					emit TransformSignal(localDir.absoluteFilePath(item_list[4]));


					UploadThroughCURL(proc,localDir.absoluteFilePath(item_list[3]),nowDate,up.data(),m_FTPConfig.host);
//					ok = false;
//					foreach(const QByteArray &info, m_CurlMsg)
//					{
//						QByteArrayList info_items = info.split(' ');

//						pct = info_items[0].toInt(&ok);
//						if(pct==100 && ok)
//						{
//							ok = true;
//							break;
//						}
//					}

//					m_CurlMsg.clear();
					while(m_ProcExitCode<0)
					{
						proc->waitForFinished();
					}

					if(m_ProcExitCode==0)
					{
						emit TransformSignal(QString("Completed uploading File %1").arg(item_list[3].data()));
						dbItems[0] = item_list[0];
						dbItems[1] = item_list[1];
						dbItems[2] = item_list[3];
						emit DBSignal(dbItems);
					}
					else
					{
						emit TransformSignal(QString("Failed to uploading File %1 -- Error:%2").arg(item_list[3].data()).arg(m_ProcExitCode));
						QString filePath = localDir.absoluteFilePath(item_list[3]);
						errUploadList.append(filePath.toStdString().c_str());
						errUploadList.append(nowDate.toStdString().c_str());
					}

					m_ProcExitCode = -1;

					UploadThroughCURL(proc,localDir.absoluteFilePath(item_list[4]),nowDate,up.data(),m_FTPConfig.host);
					while(m_ProcExitCode<0)
					{
						proc->waitForFinished();
					}
//					ok = false;
//					foreach(const QByteArray &info, m_CurlMsg)
//					{
//						QByteArrayList info_items = info.split(' ');
//						pct = info_items[0].toInt(&ok);
//						if(pct==100 && ok)
//						{
//							ok = true;
//							break;
//						}
//					}

//					m_CurlMsg.clear();

					if(m_ProcExitCode==0)
					{
						emit TransformSignal(QString("Completed uploading File %1").arg(item_list[4].data()));
						dbItems[0] = item_list[0];
						dbItems[1] = item_list[1];
						dbItems[2] = item_list[4];
						emit DBSignal(dbItems);
					}
					else
					{
						emit TransformSignal(QString("Failed to uploading File %1 -- Error:%2").arg(item_list[3].data()).arg(m_ProcExitCode));
						QString filePath = localDir.absoluteFilePath(item_list[4]);
						errUploadList.append(filePath.toStdString().c_str());
						errUploadList.append(nowDate.toStdString().c_str());
					}
					m_ProcExitCode = -1;

//					if(curl)
//					{
//						//qDebug()<<target;

//						QString filePath = qDir.absoluteFilePath(QString(item_list[3]));
//						//qDebug()<<filePath;
//						res = MainWorker::UploadFile(curl,qFile,target.data(),filePath.toStdString().c_str(),up.data());

//						if(res != CURLE_OK)
//						{
//							m_ErrMsg = curl_easy_strerror(res);
//							qxtLog->error(QString("curl_easy_perform() failed: %1").arg(m_ErrMsg.data()));
//							emit TransformSignal(QString("curl_easy_perform() failed: %1").arg(m_ErrMsg.data()));
//							errUploadList.append(filePath.toStdString().c_str());
//							errUploadList.append(target);
//						}
//						else
//						{
//							qxtLog->info(QString("Completed uploading File %1").arg(item_list[3].data()));
//							emit TransformSignal(QString("Completed uploading File %1").arg(item_list[3].data()));
//						}

//						filePath = qDir.absoluteFilePath(QString(item_list[4]));
//						res = MainWorker::UploadFile(curl,qFile,target.data(),filePath.toStdString().c_str(),up.data());

//						if(res != CURLE_OK)
//						{
//							m_ErrMsg = curl_easy_strerror(res);
//							qxtLog->error(QString("curl_easy_perform() failed: %1").arg(m_ErrMsg.data()));
//							emit TransformSignal(QString("curl_easy_perform() failed: %1").arg(m_ErrMsg.data()));
//							errUploadList.append(filePath.toStdString().c_str());
//							errUploadList.append(target);
//						}
//						else
//						{
//							qxtLog->info(QString("Completed uploading File %1").arg(item_list[4].data()));
//							emit TransformSignal(QString("Completed uploading File %1").arg(item_list[4].data()));
//							break;
//						}

//					}//end if(curl)
				}//end if(item_list.size()==5)
			}//end if(rv_len > 0 && rv_len < MAX_SIZE - 1)

		}//end if(items[0].revents & ZMQ_POLLIN)

		QThread::msleep(300);
	}//end while

	if(curl)
	{
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	zmq_close(rcv);
	zmq_close(recorder);

	delete proc;
	m_Proc = NULL;
	proc = NULL;
}



void MainWorker::OnDateChanged()
{
	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	m_DateChanged = true;
	//

	QDir qDir(m_WorkDir);

	if(qDir.cdUp())
	{
		//reset working directory
		m_WorkDir = qDir.absoluteFilePath(nowDate);
		QString logDir = qDir.absoluteFilePath("log");
		qDir.setPath(logDir);

		if(!qDir.exists())
		{
			if(qDir.mkpath(logDir))
			{
				QxtBasicFileLoggerEngine* eng = dynamic_cast<QxtBasicFileLoggerEngine*>(qxtLog->engine("app"));
				eng->setLogFileName(qDir.absoluteFilePath(QString("Kinect_%1.log").arg(nowDate)));
				qxtLog->info(QString("Upload Path in FTP is Changed to %1").arg(nowDate));
				qxtLog->info(QString("Log File Path is Changed to %1").arg(eng->logFileName()));
				qxtLog->info(QString("Working Directory is Changed to %1").arg(m_WorkDir));
			}
		}
		else
		{
			QxtBasicFileLoggerEngine* eng = dynamic_cast<QxtBasicFileLoggerEngine*>(qxtLog->engine("app"));
			eng->setLogFileName(qDir.absoluteFilePath(QString("Kinect_%1.log").arg(nowDate)));
			qxtLog->info(QString("Upload Path in FTP is Changed to %1").arg(nowDate));
			qxtLog->info(QString("Log File Path is Changed to %1").arg(eng->logFileName()));
			qxtLog->info(QString("Working Directory is Changed to %1").arg(m_WorkDir));
		}
	}
}

void MainWorker::OnFinished(int exitCode, QProcess::ExitStatus exitStat)
{
	m_ProcExitStat = exitStat;
	m_ProcExitCode = exitCode;
}

//void MainWorker::OnReadyReadStdOut()
//{
//	if(m_Proc!=NULL)
//	{
//		QByteArray info = m_Proc->readAllStandardOutput();
//		if(info.size()>0)
//		{
//			m_CurlMsg.append(info);
//			emit TransformSignal(QString(info));
//		}

//	}
//}

//void MainWorker::OnReadyReadStdErr()
//{
//	if(m_Proc!=NULL)
//	{
//		QByteArray info = m_Proc->readAllStandardError();

//		if(info.size()>0)
//		{
//			m_CurlMsg.append(info);
//			emit TransformSignal(QString(info));
//		}

//	}
//}


