#include "worker.h"
#include <zmq.h>
#include "kinectcapture.h"
#include <QDebug>
#include <QByteArray>
#include <QDir>



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

	char buf[8];

	QProcess* proc = new QProcess();

	QString inputVideoFile;
	QString outputVideoFile;
//	QString opt = " --preset=\"Normal\"";

	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

	QDir qDir;
	qDir.setPath(_kc->DirPath());

	QString converter = "HandBrakeCLI.exe";

	while(true)
	{

		if(zmq_recv(rcv,buf,7,ZMQ_DONTWAIT)>0)
		{
			break;
		}

		if(WaitForSingleObject(reinterpret_cast<HANDLE>(_kc->_evt_frame_ready), INFINITE)==WAIT_OBJECT_0)
		{

			if(_kc->Reader())
			{
				IMultiSourceFrameArrivedEventArgs *args = NULL;

				if(SUCCEEDED(_kc->Reader()->GetMultiSourceFrameArrivedEventData(_kc->_evt_frame_ready,&args)))
				{
					//qDebug()<<"Frame Arrived";
					if(_kc->ProcessArrivedFrame(args))
					{
						//Send file name to database thread
						if(_kc->CurrentFrameCount()==0)
						{
							DBItem* db_item = _kc->GetDBItem();

							qDebug()<<"Complete record file "+db_item->colorFileName;
							qDebug()<<"Complete record file "+db_item->depthFileName;

							inputVideoFile = qDir.absoluteFilePath(db_item->colorFileName);
							outputVideoFile = inputVideoFile;
							outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
							QString arguments = converter;
							arguments.append(QString(" -i %1 -o %2").arg(inputVideoFile).arg(outputVideoFile));


							proc->start(arguments);
							proc->waitForStarted();
							proc->waitForFinished();
							qDebug()<<"Complete Transform Color File:"+inputVideoFile;
							qDir.remove(db_item->colorFileName);
							db_item->colorFileName.replace("m4v","mp4",Qt::CaseInsensitive);

							inputVideoFile = qDir.absoluteFilePath(db_item->depthFileName);
							outputVideoFile = inputVideoFile;
							outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
							arguments = converter;
							arguments.append(QString(" -i %1 -o %2").arg(inputVideoFile).arg(outputVideoFile));
							qDebug()<<arguments;
							proc->start(arguments);
							proc->waitForStarted();
							proc->waitForFinished();

							qDir.remove(db_item->depthFileName);
							db_item->depthFileName.replace("m4v","mp4",Qt::CaseInsensitive);
							qDebug()<<"Complete Transform Depth File:"+inputVideoFile;

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

	zmq_close(rcv);
	zmq_close(pusher);

	delete proc;
	proc = NULL;
}

