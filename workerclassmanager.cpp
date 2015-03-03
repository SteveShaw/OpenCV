#include <QThreadPool>
#include "workerclassmanager.h"
//#include "writer.h"
#include "GrabWorker.h"
#include "TranscodeWorker.h"



WorkerManager::WorkerManager(QObject *parent)
	: QObject(parent)
	,m_ftpWorker(0)
	,m_grabWorker(0)
{

}

WorkerManager::~WorkerManager()
{

}

void WorkerManager::PrepareWorkers(KinectCapture* kc, void* ctx)
{
	m_ftpWorker = new TranscodeWorker(kc,ctx);
	m_ftpWorker->setAutoDelete(true);
	m_grabWorker = new GrabWorker(0,kc,ctx);
	m_grabWorker->setAutoDelete(true);
}



void WorkerManager::StartWorkers()
{
	QThreadPool::globalInstance()->start(m_grabWorker);
	QThreadPool::globalInstance()->start(m_ftpWorker);
}

void WorkerManager::LoadConfig(const DBConfig &dbCfg, const FTPConfig &ftpCfg)
{
	if(m_ftpWorker)
	{
		m_ftpWorker->SetDB(dbCfg);
		m_ftpWorker->SetFTP(ftpCfg);
	}
}


