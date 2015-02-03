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



int main(int argc, char *argv[])
{
	//    QCoreApplication a(argc, argv);




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

	Worker *wk = new Worker(kc.data(),ctx);
	wk->setAutoDelete(true);
	WriterBase *wr = new WriterBase(kc.data(),ctx);
	wr->setAutoDelete(true);
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


	//    return a.exec();
}
