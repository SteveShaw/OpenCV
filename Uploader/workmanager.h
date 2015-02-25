#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include <QObject>
#include "config.h"

class MainWindow;
class MonitorWorker;
class MsgQueue;
class MainWorker;


class WorkManager : public QObject
{
	Q_OBJECT
public:
	explicit WorkManager(QObject *parent=0);
	virtual ~WorkManager();

	void SetupWorker(const AppConfig &appConfig, const FTPConfig &ftpConfig);

	void StartRecorder();
	void StartWorker();


private:
	MainWindow* m_MainGUI;
//	MonitorWorker* m_MonitorWorker;

signals:
	void RecordStarted();


private:
	QString m_WorkDir;
	int m_RecordFrame;

	MsgQueue *m_MQ;
	MainWorker *m_MainWorker;
};

#endif // WORKMANAGER_H
