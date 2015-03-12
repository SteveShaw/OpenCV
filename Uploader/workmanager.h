#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include <QObject>
#include "config.h"

class MainWindow;
class DBWorker;
class MsgQueue;
class MainWorker;


class WorkManager : public QObject
{
	Q_OBJECT
public:
	explicit WorkManager(QObject *parent=0);
	virtual ~WorkManager();

	bool SetupWorker(const AppConfig &appConfig);

	void Prepare(const FTPConfig& ftpConfig, const DBConfig& dbConfig);

	void StartRecorder();
	void StartWorker();

	void SetupLogger(QString dir);

	QString ErrorMsg() const
	{
		return m_ErrMsg;
	}



private:
	MainWindow* m_MainGUI;
//	MonitorWorker* m_MonitorWorker;

signals:
	void RecordStarted();


private:
	QString m_LogDir;
	QString m_WorkDir;
	QString m_ErrMsg;
	int m_RecordFrame;
	int m_KinectID;
	int m_RoomID;

	MsgQueue *m_MQ;
	MainWorker *m_MainWorker;
	DBWorker *m_DBWorker;

	bool m_WorkerStarted;
};

#endif // WORKMANAGER_H
