#include "msgqueue.h"
#include <zmq.h>

MsgQueue::MsgQueue()
{

}

MsgQueue::~MsgQueue()
{
		zmq_close(m_sender);
		zmq_ctx_term(m_ctx);
}

void MsgQueue::Prepare()
{
	m_ctx = zmq_ctx_new();
	m_sender = zmq_socket(m_ctx,ZMQ_PUB);
	zmq_bind(m_sender,"inproc://stopper");
}

void MsgQueue::SendStopSignal()
{
	zmq_send(m_sender,"STP",3,0);

}



