#include "ftpworker.h"
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QThread>
#include <QtSql>
//#include <ftpclient.h>
#include <QFile>
#include <zmq.h>


FTPWorker::FTPWorker(KinectCapture *kc, void *ctx)
	:m_kc(kc)
	,m_ctx(ctx)
{

}

FTPWorker::~FTPWorker()
{

}

void FTPWorker::run()
{
	void* rcv = zmq_socket(m_ctx,ZMQ_SUB);
	zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
	zmq_connect(rcv,"inproc://stopper");

	void* puller = zmq_socket(m_ctx,ZMQ_PULL);
	zmq_connect(puller,"inproc://writer");

	void* informer = zmq_socket(m_ctx,ZMQ_PUB);
	zmq_bind(informer,"tcp://*:7777");

	QByteArray data_buf(MAX_SIZE,0);

	//cv::VideoWriter* cvw = nullptr;

	zmq_pollitem_t items[] = {{puller,0,ZMQ_POLLIN,0}};


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
					//inform outside program
					zmq_send(informer, data_buf.data(), rv_len, ZMQ_DONTWAIT);
				}
			}

		}
	}

	zmq_close(rcv);
	zmq_close(puller);
	zmq_close(informer);

}

bool FTPWorker::InitDB()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(m_db_cfg.host);
	db.setDatabaseName(m_db_cfg.db);
	db.setUserName(m_db_cfg.user);
	db.setPassword(m_db_cfg.pwd);

	if(!db.open())
	{
		qDebug()<<db.lastError();
		return false;
	}

	QStringList tables = db.tables();
	if (!tables.contains(m_db_cfg.tbl, Qt::CaseInsensitive))
	{
		qDebug()<<"Table "<<m_db_cfg.tbl<<" is not exist";
		return false;
	}



	return true;
}

bool FTPWorker::InsertRecord(const QList<QByteArray> &items, bool color, bool depth)
{
	QSqlQuery q;

	QString queryCmd = "insert into " + m_db_cfg.tbl;
	queryCmd += "(Start, End, FileName, DirName, Type) ";
	queryCmd += "values (?, ?, ?, ?, ?)";


	if(color)
	{
		if(!q.prepare(queryCmd))
		{
			qDebug()<<q.lastError();
			return false;
		}

		q.addBindValue(items[0]);
		q.addBindValue(items[1]);
		q.addBindValue(items[3]);
		q.addBindValue(m_ftp_cfg.path);
		q.addBindValue("Color");
		q.exec();

		qDebug()<<"Insert color record into id:"<<q.lastInsertId();

	}


	if(depth)
	{
		if(!q.prepare(queryCmd))
		{
			qDebug()<<q.lastError();
			return false;
		}

		q.addBindValue(items[0]);
		q.addBindValue(items[1]);
		q.addBindValue(items[4]);
		q.addBindValue(m_ftp_cfg.path);
		q.addBindValue("Depth");
		q.exec();

		qDebug()<<"Insert depth record into id:"<<q.lastInsertId();

	}

	return true;
}

//if(qryPrepared)
//{
//						query.bindValue(":start", item_list[0]);
//						query.bindValue(":end", item_list[1]);
//						query.bindValue(":filename", item_list[2]);
//						query.bindValue(":dirname", m_ftp_cfg.path);
//						query.bindValue(":type",item_list[4]);
//						if(query.exec())
//						{
//							qDebug()<<"INFO: Inserted "+item_list[2]+" into database";
//						}
//						else
//						{
//							qDebug()<<"ERROR: Failed to insert "+item_list[2]+" into database";
//						}
//}



