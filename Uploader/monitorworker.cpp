#include "monitorworker.h"
#include <QTimer>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

MonitorWorker::MonitorWorker(QObject *parent)
	:QObject(parent)
	,m_EvtLoop(NULL)
{
	m_Today = QDate::currentDate();
}

void MonitorWorker::run()
{
	QTimer* timer = new QTimer;
	timer->setInterval(10000);
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

bool MonitorWorker::InitDB()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(m_DBConfig.host);
	db.setDatabaseName(m_DBConfig.db);
	db.setUserName(m_DBConfig.user);
	db.setPassword(m_DBConfig.pwd);

	if(!db.open())
	{
		qDebug()<<db.lastError();
		return false;
	}

	QStringList tables = db.tables();
	if (!tables.contains(m_DBConfig.tbl, Qt::CaseInsensitive))
	{
		qDebug()<<"Table "<<m_DBConfig.tbl<<" is not exist";
		return false;
	}

	return true;
}

void MonitorWorker::timer_job()
{
	//check current time
	QDate nowDate = QDate::currentDate();
	if(nowDate!=m_Today)
	{
		m_Today = nowDate;
		emit date_changed();
	}
}

void MonitorWorker::stop()
{
	if(m_EvtLoop)
	{
		m_EvtLoop->exit();
	}
}





