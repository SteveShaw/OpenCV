#include "workerclassmanager.h"
//#include "writer.h"
#include "worker.h"
#include "ftpworker.h"
#include <QThreadPool>


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
//	m_uploader = new UploadThread(0,kc);
//	m_uploader->setAutoDelete(true);

	m_ftpWorker = new FTPWorker(kc,ctx);
	m_ftpWorker->setAutoDelete(true);
	m_grabWorker = new GrabWorker(0,kc,ctx);
	m_grabWorker->setAutoDelete(true);

	//ColorImageWriter* clr_wrt = new ColorImageWriter(kc.data(),ctx);
	//clr_wrt->setAutoDelete(true);
	//QThreadPool::globalInstance()->start(clr_wrt);

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


