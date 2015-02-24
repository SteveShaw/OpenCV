#include "filewatcher.h"
#include <QDebug>

FileWatcher::FileWatcher(QObject *parent)
	:QObject(parent)
	,m_evtLoop(nullptr)
	,m_timer(nullptr)
{
}

FileWatcher::~FileWatcher()
{

}

void FileWatcher::run()
{
	if(m_timer==nullptr)
	{
		m_timer = new QTimer;
		m_timer->setInterval(15000);
		connect(m_timer,SIGNAL(timeout()),this,SLOT(timer_job()));
		m_timer->start();
	}

	m_evtLoop = new QEventLoop();
	m_evtLoop->exec();

	delete m_evtLoop;

	if(m_timer->isActive())
	{
		m_timer->stop();
	}

	delete m_timer;
}

void FileWatcher::timer_job()
{
	m_mutex.lock();
	qDebug()<<m_filePathList;
	m_mutex.unlock();
}

void FileWatcher::latest_file(QString path)
{
	m_mutex.lock();
	m_filePathList.append(path);
	m_mutex.unlock();
}

