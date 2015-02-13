#include "writer.h"
#include <QDebug>
#include <zmq.h>
#include "kinectcapture.h"
#include <QFileDevice>
#include <QDateTime>
#include <QDir>
#include <ftpclient.h>



#define MAX_SIZE 512

DBThread::DBThread(KinectCapture *kc, void *ctx)
	:_kc(kc)
	,_ctx(ctx)
	//,_mmf("d:\\Test.avi")
{
	//_mmf.open(QFile::WriteOnly);
	//    _mmf.map(0,)
}

DBThread::~DBThread()
{

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



void DBThread::run()
{
	void* rcv = zmq_socket(_ctx,ZMQ_SUB);
	zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
	zmq_connect(rcv,"inproc://stopper");

	void* puller = zmq_socket(_ctx,ZMQ_PULL);
	zmq_connect(puller,"inproc://writer");

	QByteArray data_buf(MAX_SIZE,0);

	//cv::VideoWriter* cvw = nullptr;

	zmq_pollitem_t items[] = {
		{puller,0,ZMQ_POLLIN,0}
	};

	std::string ftp_host = "129.105.36.214";
	std::string user = "jianshao";
	std::string pwd = "111111";

	FTPClient *ftp = new FTPClient(ftp_host.c_str(),user.c_str(),pwd.c_str());

	while(true)
	{

		if(zmq_recv(rcv,data_buf.data(),MAX_SIZE-1,ZMQ_DONTWAIT)>0)
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
			int rv_len = zmq_recv(puller,data_buf.data(),MAX_SIZE-1,ZMQ_DONTWAIT);
			if(rv_len > 0 && rv_len < MAX_SIZE-1)
			{
				data_buf[rv_len]='\0';


				QList<QByteArray> item_list = data_buf.split(';');

				if(item_list.size()==5)
				{
					qDebug()<<"Uploading to FTP";

					qDebug()<<data_buf;

					QByteArray local_path = item_list[3];
					local_path.append("/");
					local_path.append(item_list[2]);

					QByteArray remote_path = "/home/jianshao/";
					remote_path.append(item_list[2]);
					ftp->LogIn();
					int upload_bytes = ftp->Upload(remote_path.data(),local_path.data());
					ftp->Close();
					qDebug()<<upload_bytes<<" has uploaded";

				}
			}


		}
	}

	zmq_close(rcv);
	zmq_close(puller);

}

bool DBThread::SetDB(const QString& hostname, const QString& dbname, const QString& user, const QString& pwd)
{
	m_db = QSqlDatabase::addDatabase("QMYSQL");
	m_db.setHostName(hostname);
	m_db.setDatabaseName(dbname);
	m_db.setUserName(user);
	m_db.setPassword(pwd);

	return m_db.open();
}

