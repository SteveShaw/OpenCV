#include "GrabWorker.h"
#include <zmq.h>
#include "kinectcapture.h"
#include <QDebug>
#include <QByteArray>
#include <QDir>
#include <QThread>

//#include <QxtCore/QxtBasicFileLoggerEngine>
//#include <QxtCore/QxtLogger>

GrabWorker::~GrabWorker()
{

}

void GrabWorker::run()
{
	void* rcv = zmq_socket(_ctx,ZMQ_SUB);
	zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
	zmq_connect(rcv,"inproc://stopper");

	void* pusher = zmq_socket(_ctx,ZMQ_PUSH);
	zmq_bind(pusher,"inproc://writer");

	void* pauser = zmq_socket(_ctx,ZMQ_SUB);
	zmq_setsockopt(pauser,ZMQ_SUBSCRIBE,0,0);
	zmq_connect(pauser,"tcp://localhost:9988");//pause the current thread recording

	char buf[8];

	bool pause = false;
//	QProcess *proc = new QProcess();
//	//proc->setProcessChannelMode(QProcess::ForwardedChannels);

//	QString inputVideoFile;
//	QString outputVideoFile;

//	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
//	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

//	QDir qDir;
//	qDir.setPath(_kc->DirPath());

	while(true)
	{

		if(zmq_recv(rcv,buf,7,ZMQ_DONTWAIT)>0)
		{
			break;
		}

		int len = zmq_recv(pauser,buf,7,ZMQ_DONTWAIT);

		if(len>0 &&  len < 7)
		{
			buf[len] = '\0';
			QByteArray inst(buf);
			if(inst=="PAU")
			{
				qDebug()<<"Record Paused";
				pause = true;
			}
			else if(inst=="RES")
			{
				qDebug()<<"Record Resumed";
				pause = false;
			}
		}

		if(WaitForSingleObject(reinterpret_cast<HANDLE>(_kc->_evt_frame_ready), INFINITE)==WAIT_OBJECT_0)
		{

			if(_kc->Reader())
			{
				IMultiSourceFrameArrivedEventArgs *args = NULL;

				if(SUCCEEDED(_kc->Reader()->GetMultiSourceFrameArrivedEventData(_kc->_evt_frame_ready,&args)))
				{
					//qDebug()<<"Frame Arrived";
					if(!pause && _kc->ProcessArrivedFrame(args))
					{
						//Send file name to database thread
						if(_kc->CurrentFrameCount()==0)
						{
							DBItem* db_item = _kc->GetDBItem();

							qDebug()<<"Complete record file "+db_item->colorFileName;
							qDebug()<<"Complete record file "+db_item->depthFileName;

//							inputVideoFile = qDir.absoluteFilePath(db_item->colorFileName);
//							outputVideoFile = inputVideoFile;
//							outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
//							GrabWorker::Transcode(proc,inputVideoFile,outputVideoFile);
//							qDebug()<<"Complete Transform Color File:"+inputVideoFile;
//							qDir.remove(db_item->colorFileName);
//							db_item->colorFileName.replace("m4v","mp4",Qt::CaseInsensitive);

//							inputVideoFile = qDir.absoluteFilePath(db_item->depthFileName);
//							outputVideoFile = inputVideoFile;
//							outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
//							qDebug()<<"Complete Transform Depth File:"+inputVideoFile;
//							GrabWorker::Transcode(proc,inputVideoFile,outputVideoFile);
//							qDir.remove(db_item->depthFileName);
//							db_item->depthFileName.replace("m4v","mp4",Qt::CaseInsensitive);


							db_item->ComposeRow();
							zmq_send(pusher,db_item->row,db_item->row.size(),0);
						}
					}

				}

				if(args!=NULL)
				{
					args->Release();
					args = NULL;
				}
			}
		}
	}

	qDebug()<<"End Thread";

	_kc->Release();

//	qxtLog->info("Kinect Recorder Exit");

	zmq_close(rcv);
	zmq_close(pusher);
	zmq_close(pauser);
}




