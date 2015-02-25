#include "workmanager.h"
#include "mainwindow.h"
#include "monitorworker.h"
#include <QThreadPool>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#include "msgqueue.h"
#include "mainworker.h"




WorkManager::WorkManager(QObject *parent)
	:QObject(parent)
	,m_MainGUI(new MainWindow(0))
//	,m_MonitorWorker(new MonitorWorker(0))
	,m_MainWorker(new MainWorker(0))
	,m_WorkDir("D:/Video")
	,m_RecordFrame(100)
	,m_MQ(new MsgQueue)
{
	//m_MonitorWorker->setAutoDelete(true);
	m_MainWorker->setAutoDelete(true);
}

WorkManager::~WorkManager()
{
	m_MQ->SendStopSignal();
	QThreadPool::globalInstance()->waitForDone();

	delete m_MainGUI;
	delete m_MQ;
}

void WorkManager::SetupWorker(const AppConfig& appConfig, const FTPConfig& ftpConfig)
{
	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	m_WorkDir = appConfig.videoPath;
	QString watchPath = m_WorkDir;
	watchPath += "/";
	watchPath += nowDate;
	qDebug()<<watchPath;
//	m_MainGUI->SetupWatcher(watchPath.toStdString().c_str());
//	connect(m_MonitorWorker,SIGNAL(timer_signal()),m_MainGUI,SLOT(timer_signaled()));
//	connect(m_MainGUI,SIGNAL(stop_signal()),m_MonitorWorker,SLOT(stop()));
	connect(this,SIGNAL(RecordStarted()),m_MainGUI,SLOT(OnRecordStarted()));
	connect(m_MainWorker,SIGNAL(TransformSignal(QString)),m_MainGUI,SLOT(OnTransformSignal(QString)));
	m_MQ->Prepare();
	m_MainWorker->SetContext(m_MQ->GetContext());
	m_MainWorker->SetWorkingDirectory(watchPath.toStdString().c_str());
	m_MainWorker->SetVideoConverter("E:\\Projects\\bin\\HandBrakeCLI.exe");
	m_MainWorker->SetFTPConfig(ftpConfig);

}

void WorkManager::StartRecorder()
{
	QString cmd = "KinectRecord ";
	cmd += m_WorkDir;
	cmd += " ";
	cmd += QString::number(m_RecordFrame);
	if(QProcess::startDetached(cmd))
	{
		emit RecordStarted();
	}
	else
	{
		qDebug()<<"Failed to start recorder";
	}
}

void WorkManager::StartWorker()
{
	m_MainGUI->show();
	QThreadPool::globalInstance()->start(m_MainWorker,QThread::LowPriority);
	//QThreadPool::globalInstance()->start(m_MonitorWorker,QThread::LowPriority);
}



