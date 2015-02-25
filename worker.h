#ifndef WORKER_H
#define WORKER_H


#include <QObject>
#include <QRunnable>
#include <QProcess>

class KinectCapture;

class GrabWorker : public QObject, public QRunnable
{
	Q_OBJECT
	Q_ENUMS(QProcess::ExitStatus)
	Q_ENUMS(QProcess::ProcessError)

public:
		GrabWorker(QObject*parent, KinectCapture *kc, void* ctx)
				:QObject(parent)
				,_ctx(ctx)
    {
        _kc = kc;
    }

    ~GrabWorker();

protected:
    void run();

private:
    KinectCapture *_kc;
    void* _ctx;

signals:
		void new_item(QString, QString, QString, QString, QString);
};

#endif // WORKER_H
