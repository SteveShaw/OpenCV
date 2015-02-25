#ifndef MONITORWORKER_H
#define MONITORWORKER_H

#include <QObject>
#include <QRunnable>
#include <QEventLoop>

class MonitorWorker : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit MonitorWorker(QObject* parent=0);
	virtual ~MonitorWorker(){}

protected:
	void run();

private:
	QEventLoop* m_EvtLoop;

public slots:
	void timer_job();
	void stop();

signals:
	void timer_signal();

};

#endif // MONITORWORKER_H
