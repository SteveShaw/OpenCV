#ifndef MONITORWORKER_H
#define MONITORWORKER_H

#include <QObject>
#include <QRunnable>
#include <QEventLoop>
#include <QDate>
#include <QStringList>
#include <QSqlDatabase>
#include "config.h"

class DBWorker : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit DBWorker(QObject* parent=0);
	virtual ~DBWorker()
	{
		if(m_SqlDB.isOpen())
		{
			m_SqlDB.close();
		}
	}

	bool InitDB();
	void Stop();

	void SetDBConfig(const DBConfig &cfg)
	{
		m_DBConfig.db = cfg.db;
		m_DBConfig.host = cfg.host;
		m_DBConfig.pwd = cfg.pwd;
		m_DBConfig.tbl = cfg.tbl;
		m_DBConfig.user = cfg.user;
	}

	void SetKinectNumber(int kn)
	{
		m_KinectNumber = kn;
	}

	void SetRoomNumber(int rn)
	{
		m_RoomNumber = rn;
	}

protected:
	void run();

protected slots:
	void OnDBSignal(QStringList val);

private:
	QEventLoop* m_EvtLoop;
//	QDate m_Today;
	DBConfig m_DBConfig;
	bool m_IsDBWork;
	int m_KinectNumber;
	int m_RoomNumber;

	QString m_DTFmt;


	QSqlDatabase m_SqlDB;

};

#endif // MONITORWORKER_H
