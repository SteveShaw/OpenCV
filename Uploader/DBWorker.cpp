#include "DBWorker.h"
#include <QTimer>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

DBWorker::DBWorker(QObject *parent)
	:QObject(parent)
	,m_EvtLoop(NULL)
	,m_IsDBWork(false)
	,m_DTFmt("yyyyMMdd_HHmmss_zzz")
{
}

void DBWorker::run()
{
	//	QTimer* timer = new QTimer;
	//	timer->setInterval(10000);
	//	connect(timer,SIGNAL(timeout()),this,SLOT(timer_job()));
	//	timer->start();

	m_EvtLoop = new QEventLoop();
	m_EvtLoop->exec();

	delete m_EvtLoop;

	//	if(timer->isActive())
	//	{
	//		timer->stop();
	//	}

	//	delete timer;
}

void DBWorker::OnDBSignal(QStringList val)
{
	if(!m_IsDBWork || val.size()!=3)
	{
		return;
	}

	QDateTime start = QDateTime::fromString(val[0],m_DTFmt);
	QDateTime end = QDateTime::fromString(val[1],m_DTFmt);
	QString &filePath = val[2];

	QSqlQuery query;
	query.prepare("INSERT INTO KinectVideo (Start, End, RoomID, KinectID, FilePath) "
								"VALUES (:start, :end, :room, :kinect, :path)");
	query.bindValue(":start", start);
	query.bindValue(":end", end);
	query.bindValue(":room", m_RoomNumber);
	query.bindValue(":kinect", m_KinectNumber);
	query.bindValue(":path",filePath);
	query.exec();
}

bool DBWorker::InitDB()
{
	m_SqlDB = QSqlDatabase::addDatabase("QMYSQL");
	m_SqlDB.setHostName(m_DBConfig.host);
	m_SqlDB.setDatabaseName(m_DBConfig.db);
	m_SqlDB.setUserName(m_DBConfig.user);
	m_SqlDB.setPassword(m_DBConfig.pwd);

	if(!m_SqlDB.open())
	{
		qDebug()<<m_SqlDB.lastError();
		m_IsDBWork = false;
		return false;
	}

	QStringList tables = m_SqlDB.tables();
	if (!tables.contains(m_DBConfig.tbl, Qt::CaseInsensitive))
	{
		qDebug()<<"Table "<<m_DBConfig.tbl<<" is not exist";
		m_IsDBWork = false;
		return false;
	}

	m_IsDBWork = true;

	return true;
}


void DBWorker::Stop()
{
	if(m_EvtLoop)
	{
		m_EvtLoop->exit();
	}
}





