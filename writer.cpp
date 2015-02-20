#include "writer.h"
#include <QDebug>
//#include <zmq.h>
#include "kinectcapture.h"
#include <QFileDevice>
//#include <QDateTime>
#include <QDir>
#include "ftpupload.h"
#include <QTimer>
//#include <ftpclient.h>
//#include <QSqlQuery>




//#define MAX_SIZE 512

UploadThread::UploadThread(QObject *parent, KinectCapture *kc)
	:QObject(parent)
	,_kc(kc)
	//,_mmf("d:\\Test.avi")
{
	//_mmf.open(QFile::WriteOnly);
	//    _mmf.map(0,)
	m_ftpUploader = new FTPUploader();
}

UploadThread::~UploadThread()
{
	delete m_ftpUploader;
}

void UploadThread::SetFTP(const FTPConfig &cfg)
{
	m_ftp_cfg.user = cfg.user;
	m_ftp_cfg.pwd = cfg.pwd;
	m_ftp_cfg.path = cfg.path;
	m_ftp_cfg.host = cfg.host;


	m_ftpUploader->SetConfig(m_ftp_cfg);
}

//void WriterBase::InitializeMessageQueue()
//{
//    _sock = zmq_socket(_ctx,ZMQ_SUB);
//    zmq_setsockopt(_sock,ZMQ_SUBSCRIBE,0,0);
//    zmq_connect(_sock,"inproc://stopper");
//}

//bool WriterBase::IsStopped()
//{
//    int len = zmq_recv(_sock,_buf,7,ZMQ_DONTWAIT);
//    return len > 0;
//}

//void WriterBase::ReleaseMessageQueue()
//{
//    zmq_close(_sock);
//    //return true;
//}



void UploadThread::run()
{
//	void* rcv = zmq_socket(_ctx,ZMQ_SUB);
//	zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
//	zmq_connect(rcv,"inproc://stopper");

//	void* puller = zmq_socket(_ctx,ZMQ_PULL);
//	zmq_connect(puller,"inproc://writer");

//	QByteArray data_buf(MAX_SIZE,0);

	//cv::VideoWriter* cvw = nullptr;

//	zmq_pollitem_t items[] = {
//		{puller,0,ZMQ_POLLIN,0}
//	};




	m_evtLoop = new QEventLoop();
	m_evtLoop->exec();

	delete m_evtLoop;



//	std::string ftp_host = m_ftp_cfg.host.toStdString();//"129.105.36.214";
//	std::string user = m_ftp_cfg.user.toStdString();//"jianshao";
//	std::string pwd = m_ftp_cfg.pwd.toStdString();//"111111";

	//"/home/jianshao/";


//	FTPClient *ftp = new FTPClient(ftp_host.c_str(),user.c_str(),pwd.c_str());




	//	QSqlQuery query(m_db);
	//	QString queryCmd = "INSERT INTO " + m_db_cfg.tbl;
	//	queryCmd += " (Start, End, FileName, DirName, Type) ";
	//	queryCmd += "VALUES (:start, :end, :filename, :dirname, :type)";
	//	bool qryPrepared = true;//query.prepare(queryCmd);
	//	qDebug()<<qryPrepared;


//	while(true)
//	{
//		if(zmq_recv(rcv,data_buf.data(),MAX_SIZE-1,ZMQ_DONTWAIT)>0)
//		{
//			break;
//		}

//		int rc = zmq_poll(items,1,3000);

//		if(rc==-1)
//		{
//			break;
//		}

//		if(items[0].revents & ZMQ_POLLIN)
//		{
//			int rv_len = zmq_recv(puller,data_buf.data(),MAX_SIZE-1,ZMQ_DONTWAIT);
//			if(rv_len > 0 && rv_len < MAX_SIZE-1)
//			{
//				data_buf[rv_len]='\0';


//				QList<QByteArray> item_list = data_buf.split(';');
//				qDebug()<<item_list.size();

//				if(item_list.size()==5)
//				{
//					qDebug()<<"Uploading to FTP";

//					qDebug()<<data_buf;

//					QByteArray local_path = item_list[2];
//					local_path.append("/");
//					local_path.append(item_list[3]);


////					QString remote_path = m_ftp_cfg.path;
////					remote_path.append(item_list[3]);

//					ftpUploader.Prepare(local_path.data(),item_list[3].data());

////					QTimer::singleShot(0,&ftpUploader, SLOT(execute()));

//					ftpUploader.DoUpload();
////					ftp->LogIn();
////					int upload_bytes = ftp->Upload(remote_path.toStdString().c_str(),local_path.data());
////					qDebug()<<upload_bytes<<" has uploaded to "<<remote_path;
////					ftp->Close();

//					local_path = item_list[2];
//					local_path.append("/");
//					local_path.append(item_list[4]);

//					ftpUploader.Prepare(local_path.data(),item_list[4].data());
////					QTimer::singleShot(0,&ftpUploader, SLOT(execute()));
//					ftpUploader.DoUpload();


////					remote_path = m_ftp_cfg.path;
////					remote_path.append(item_list[4]);

////					ftp->LogIn();
////					upload_bytes = ftp->Upload(remote_path.toStdString().c_str(),local_path.data());
////					ftp->Close();

////					qDebug()<<upload_bytes<<" has uploaded to "<<remote_path;

//					//if(qryPrepared)
//					//{
//					//						query.bindValue(":start", item_list[0]);
//					//						query.bindValue(":end", item_list[1]);
//					//						query.bindValue(":filename", item_list[2]);
//					//						query.bindValue(":dirname", m_ftp_cfg.path);
//					//						query.bindValue(":type",item_list[4]);
//					//						if(query.exec())
//					//						{
//					//							qDebug()<<"INFO: Inserted "+item_list[2]+" into database";
//					//						}
//					//						else
//					//						{
//					//							qDebug()<<"ERROR: Failed to insert "+item_list[2]+" into database";
//					//						}
//					//}

//				}
//			}


//		}
//	}

//	zmq_close(rcv);
//	zmq_close(puller);

}

void UploadThread::upload_start(QString start, QString end, QString dir, QString color_file, QString depth_file)
{
	qDebug()<<"Start:"<<start<<";End:"<<end<<";Dir:"<<dir<<";File:"<<color_file<<" "<<depth_file;
	QString localPath = dir+"/"+color_file;
	m_ftpUploader->Prepare(localPath.toStdString().c_str(),color_file.toStdString().c_str());
	QTimer::singleShot(0,m_ftpUploader,SLOT(execute()));

	localPath = dir+"/"+depth_file;
	m_ftpUploader->Prepare(localPath.toStdString().c_str(),depth_file.toStdString().c_str());
	QTimer::singleShot(0,m_ftpUploader,SLOT(execute()));
}

//bool DBThread::SetDB(const QString& hostname, const QString& dbname, const QString& user, const QString& pwd)
//{
//	m_db = QSqlDatabase::addDatabase("QMYSQL");
//	m_db.setHostName(hostname);
//	m_db.setDatabaseName(dbname);
//	m_db.setUserName(user);
//	m_db.setPassword(pwd);

//	return m_db.open();
//}

