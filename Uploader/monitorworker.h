#ifndef MONITORWORKER_H
#define MONITORWORKER_H

#include <QObject>
#include <QRunnable>
#include <QEventLoop>
#include <QDate>

class MonitorWorker : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit MonitorWorker(QObject* parent=0);
	virtual ~MonitorWorker(){}

	void GetToday()
	{
		m_Today = QDate::currentDate();
	}

protected:
	void run();

private:
	QEventLoop* m_EvtLoop;
	QDate m_Today;

public slots:
	void timer_job();
	void stop();

signals:
	void date_changed();

};

#endif // MONITORWORKER_H
