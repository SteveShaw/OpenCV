#include "monitorworker.h"
#include <QTimer>
#include <QDebug>

MonitorWorker::MonitorWorker(QObject *parent)
	:QObject(parent)
	,m_EvtLoop(NULL)
{

}

void MonitorWorker::run()
{
	QTimer* timer = new QTimer;
	timer->setInterval(150000);
	connect(timer,SIGNAL(timeout()),this,SLOT(timer_job()));
	timer->start();

	m_EvtLoop = new QEventLoop();
	m_EvtLoop->exec();

	delete m_EvtLoop;

	if(timer->isActive())
	{
		timer->stop();
	}

	delete timer;
}

void MonitorWorker::timer_job()
{
	emit timer_signal();
}

void MonitorWorker::stop()
{
	if(m_EvtLoop)
	{
		m_EvtLoop->exit();
	}
}





