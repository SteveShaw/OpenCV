#include "writer.h"
#include <QDebug>
#include <zmq.h>
#include "kinectcapture.h"
#include <QFileDevice>
#include <QDateTime>
#include <QDir>



WriterBase::WriterBase(KinectCapture *kc, void *ctx)
    :_kc(kc)
    ,_ctx(ctx)
    //,_mmf("d:\\Test.avi")
{
    //_mmf.open(QFile::WriteOnly);
    //    _mmf.map(0,)
}

WriterBase::~WriterBase()
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



void WriterBase::run()
{
    void* rcv = zmq_socket(_ctx,ZMQ_SUB);
    zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);
    zmq_connect(rcv,"inproc://stopper");

    void* puller = zmq_socket(_ctx,ZMQ_PULL);
    zmq_connect(puller,"inproc://writer");

    char buf[8];

    //cv::VideoWriter* cvw = nullptr;


    zmq_pollitem_t items[] = {
        {puller,0,ZMQ_POLLIN,0}
    };

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
            if(zmq_recv(puller,buf,7,ZMQ_DONTWAIT)>0)
            {
            }
        }
    }


    zmq_close(rcv);
    zmq_close(puller);

}

