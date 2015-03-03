#include "TranscodeWorker.h"
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QThread>
//#include <ftpclient.h>
#include <QDir>
#include <zmq.h>
#include "kinectcapture.h"



TranscodeWorker::TranscodeWorker(KinectCapture *kc, void *ctx)
	:m_kc(kc)
	,m_ctx(ctx)
{

}

TranscodeWorker::~TranscodeWorker()
{

}

void TranscodeWorker::run()
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

	QProcess *proc = new QProcess();
	//proc->setProcessChannelMode(QProcess::ForwardedChannels);

	QString inputVideoFile;
	QString outputVideoFile;

	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

	QDir qDir;
	qDir.setPath(m_kc->DirPath());

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
					inputVideoFile = qDir.absoluteFilePath(QString(item_list[3]));
					outputVideoFile = inputVideoFile;
					outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
					TranscodeWorker::Transcode(proc,inputVideoFile,outputVideoFile);
					qDebug()<<"Complete Transform Color File:"+inputVideoFile;
					qDir.remove(QString(item_list[3]));
					item_list[3].replace(QByteArray("m4v"),QByteArray("mp4"));

					inputVideoFile = qDir.absoluteFilePath(QString(item_list[4]));
					outputVideoFile = inputVideoFile;
					outputVideoFile.replace("m4v","mp4",Qt::CaseInsensitive);
					TranscodeWorker::Transcode(proc,inputVideoFile,outputVideoFile);
					qDebug()<<"Complete Transform Depth File:"+inputVideoFile;
					qDir.remove(QString(item_list[4]));
					item_list[4].replace(QByteArray("m4v"),QByteArray("mp4"));

					//inform outside program
					zmq_send(informer, item_list.join(';').data(), rv_len, ZMQ_DONTWAIT);
				}
			}

		}
	}

	zmq_close(rcv);
	zmq_close(puller);
	zmq_close(informer);

	delete proc;
	proc = NULL;

}

void TranscodeWorker::Transcode(QProcess *proc, const QString &input, const QString &output)
{
	QString arguments = "FFMPEG.exe";
	arguments.append(QString(" -i %1 -c:v libx264 -preset fast %2 -y").arg(input).arg(output));
	qDebug()<<arguments;
	proc->start(arguments);
	proc->waitForStarted();
	proc->waitForBytesWritten();
	proc->waitForReadyRead();

	proc->waitForFinished();
	while(proc->processId()>0)
	{
		proc->waitForFinished();
	}
	proc->closeWriteChannel();
}



