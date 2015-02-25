#ifndef MSGQUEUE_H
#define MSGQUEUE_H


class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();

	void Prepare();

	void* GetSender()
	{
		return m_sender;
	}

	void* GetContext()
	{
		return m_ctx;
	}

	void SendStopSignal();

private:
	void* m_ctx;
	void* m_sender;

};

#endif // MSGQUEUE_H
