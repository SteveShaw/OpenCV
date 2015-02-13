#include <QCoreApplication>
#include <QDebug>
#include <QThreadPool>
#include <QScopedPointer>
#include <zmq.h>
#include "worker.h"
#include "writer.h"
#include "kinectcapture.h"
#include <QSettings>
#include <QString>
//#include "colorimagewriter.h"

void LoadDBSettings(const char* path)
{
	QSettings db_config(QString(path),QSettings::IniFormat);
	QString host = db_config.value("host").toString();
	QString user = db_config.value("user").toString();
	QString pwd = db_config.value("password").toString();
	QString db_name = db_config.value("db").toString();

	qDebug()<<host;
	qDebug()<<user;
	qDebug()<<pwd;
	qDebug()<<db_name;
}

int main(int argc, char *argv[])
{


//	QString path = QCoreApplication::applicationDirPath()+"/config.ini";
//	qDebug()<<path;
//	LoadDBSettings(path.toStdString().c_str());

//	return a.exec();


	QScopedPointer<KinectCapture> kc(new KinectCapture);



	void* ctx = zmq_ctx_new();
	void* sender = zmq_socket(ctx,ZMQ_PUB);
	zmq_bind(sender,"inproc://stopper");


	if(!kc->Initialize())
	{
		qDebug()<<"Cannot Initialized Kinect";

		return -1;
		//        QScopedPointer<Worker> sp_worker(new Worker(&kc,ctx));
	}


	if(argc>=2)
	{
		kc->SetSaveDirectory(argv[1]);
	}

	if(argc>=3)
	{
		kc->SetFrameCount(QString(argv[2]).toInt());
	}

	DBThread *wr = new DBThread(kc.data(),ctx);
	wr->setAutoDelete(true);
	Worker *wk = new Worker(kc.data(),ctx);
	wk->setAutoDelete(true);
	//ColorImageWriter* clr_wrt = new ColorImageWriter(kc.data(),ctx);
	//clr_wrt->setAutoDelete(true);
	//QThreadPool::globalInstance()->start(clr_wrt);

	QThreadPool::globalInstance()->start(wk);
	QThreadPool::globalInstance()->start(wr);

	getchar();


	zmq_send(sender,"STP",3,0);

	QThreadPool::globalInstance()->waitForDone();


	zmq_close(sender);
	zmq_ctx_term(ctx);

	//    delete wk;
	//    delete kc;

}
