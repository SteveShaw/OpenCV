#include "worker.h"
#include <zmq.h>
#include "kinectcapture.h"
#include <QDebug>



Worker::~Worker()
{

}

void Worker::run()
{
    void* rcv = zmq_socket(_ctx,ZMQ_SUB);
    zmq_setsockopt(rcv,ZMQ_SUBSCRIBE,0,0);

    zmq_connect(rcv,"inproc://stopper");

    void* pusher = zmq_socket(_ctx,ZMQ_PUSH);
    zmq_bind(pusher,"inproc://writer");

    char buf[8];

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
                IMultiSourceFrameArrivedEventArgs *args = nullptr;

                if(SUCCEEDED(_kc->Reader()->GetMultiSourceFrameArrivedEventData(_kc->_evt_frame_ready,&args)))
                {
                    //qDebug()<<"Frame Arrived";

                    if(_kc->ProcessArrivedFrame(args))
                    {
											//Send file name to database thread
												//zmq_send(pusher,"OK",2,0);
                    }
                }

                args->Release();
            }
        }
    }

    qDebug()<<"End Thread";

    zmq_close(rcv);
    zmq_close(pusher);
}

