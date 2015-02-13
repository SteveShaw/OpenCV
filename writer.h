#ifndef WRITER_H
#define WRITER_H

#include <QRunnable>
#include <QSqlDatabase>

class KinectCapture;

class DBThread : public QRunnable
{
public:
	explicit DBThread(KinectCapture* kc, void* ctx);
	~DBThread();

	//void InitializeMessageQueue();
	//bool IsStopped();
	//void ReleaseMessageQueue();

	protected:
		void run();

	bool SetDB(const QString &hostname, const QString &dbname, const QString &user, const QString &pwd);

private:
	KinectCapture *_kc;

	void* _ctx;

	QSqlDatabase m_db;
	//void* _sock;

	//    char _buf[8];


};

#endif // WRITER_H
