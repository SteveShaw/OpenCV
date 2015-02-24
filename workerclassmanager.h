#ifndef WORKERCLASSMANAGER_H
#define WORKERCLASSMANAGER_H

#include <QObject>
#include "dbitem.h"
class GrabWorker;
//class UploadThread;
class FTPWorker;
class KinectCapture;

class WorkerManager : public QObject
{
	Q_OBJECT
public:
	explicit WorkerManager(QObject *parent = 0);
	~WorkerManager();

	void PrepareWorkers(KinectCapture *kc, void *ctx);

	void StartWorkers();

	void LoadConfig(const DBConfig& dbCfg, const FTPConfig &ftpCfg);

signals:

public slots:

public:
	GrabWorker* m_grabWorker;
	FTPWorker* m_ftpWorker;
};

#endif // WORKERCLASSMANAGER_H
