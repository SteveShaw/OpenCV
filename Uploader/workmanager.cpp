#include "workmanager.h"
#include "mainwindow.h"
#include "monitorworker.h"
#include <QThreadPool>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#include <QxtCore/QxtBasicFileLoggerEngine>
#include <QxtCore/QxtLogger>
#include "msgqueue.h"
#include "mainworker.h"
#include "monitorworker.h"




WorkManager::WorkManager(QObject *parent)
	:QObject(parent)
	,m_MainGUI(new MainWindow(0))
//	,m_MonitorWorker(new MonitorWorker(0))
	,m_RecordFrame(100)
	,m_MQ(new MsgQueue)
	,m_MainWorker(NULL)
	,m_WorkerStarted(false)

{
	//m_MonitorWorker->setAutoDelete(true);
}

WorkManager::~WorkManager()
{
	if(m_WorkerStarted)
	{
		m_MQ->SendStopSignal();
		m_MonitorWorker->stop();
		QThreadPool::globalInstance()->waitForDone();
		m_WorkerStarted = false;
	}

	qxtLog->info("Monitor Exist");

	delete m_MainGUI;
	delete m_MQ;
}

bool WorkManager::SetupWorker(const AppConfig& appConfig)
{
	m_KinectID = appConfig.kinectID;
	m_RoomID = appConfig.roomID;


	//create log directory
	m_WorkDir = appConfig.videoPath;
	QDir qDir(m_WorkDir);
	m_LogDir = qDir.absoluteFilePath("log");
	qDir.setPath(m_LogDir);
	if(!qDir.exists())
	{
		if(!qDir.mkpath(m_LogDir))
		{
			m_ErrMsg = "Cannot Create Log Directory";
			return false;
		}

	}

	//create save direcotry
	qDir.setPath(m_WorkDir);
	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	m_WorkDir = qDir.absoluteFilePath(nowDate);
	qDir.setPath(m_WorkDir);
	if(!qDir.exists())
	{
		if (!qDir.mkpath(m_WorkDir))
		{
			m_ErrMsg = QString("Cannot Create Directory %1").arg(m_WorkDir);
			return false;
		}
	}


	return true;
}

void WorkManager::Prepare(const FTPConfig& ftpConfig)
{
	//	m_MainGUI->SetupWatcher(watchPath.toStdString().c_str());
	//	connect(m_MonitorWorker,SIGNAL(timer_signal()),m_MainGUI,SLOT(timer_signaled()));
	//	connect(m_MainGUI,SIGNAL(stop_signal()),m_MonitorWorker,SLOT(stop()));
		m_MainWorker = new MainWorker(0);
		m_MonitorWorker = new MonitorWorker(0);

		connect(this,SIGNAL(RecordStarted()),m_MainGUI,SLOT(OnRecordStarted()));
		connect(m_MainWorker,SIGNAL(TransformSignal(QString)),m_MainGUI,SLOT(OnTransformSignal(QString)));
		connect(m_MonitorWorker,SIGNAL(date_changed()),m_MainWorker,SLOT(OnDateChanged()));

		m_MQ->Prepare();
		m_MainWorker->SetContext(m_MQ->GetContext());
		m_MainWorker->SetWorkingDirectory(m_WorkDir.toStdString().c_str());
		m_MainWorker->SetFTPConfig(ftpConfig);
		m_MainWorker->setAutoDelete(true);
		m_MonitorWorker->setAutoDelete(true);

		SetupLogger(m_LogDir);
}

void WorkManager::StartRecorder()
{
	QString cmd = "KinectRecord ";
//	cmd += m_WorkDir;
//	cmd += " ";
//	cmd += QString::number(m_RecordFrame);
	if(QProcess::startDetached(cmd))
	{
		qxtLog->info("Recorder Started");
		emit RecordStarted();
	}
	else
	{
//		qxtLog->debug("Cannot start recorder");
		qxtLog->error("Cannot start recorder");
	}
}

void WorkManager::StartWorker()
{
	m_MainGUI->show();
	if(!m_WorkerStarted)
	{
		QThreadPool::globalInstance()->start(m_MainWorker,QThread::LowPriority);
		QThreadPool::globalInstance()->start(m_MonitorWorker,QThread::LowPriority);
		m_WorkerStarted = true;
	}
}

void WorkManager::SetupLogger(QString dir)
{
//	QString debugFile = path;
//	debugFile.append("\\");
//	debugFile.append("KinectDebug.log");

//	QxtBasicFileLoggerEngine *dbg  = new QxtBasicFileLoggerEngine(debugFile);


	QDir qDir(dir);
	QString nowDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	QxtBasicFileLoggerEngine *info = new QxtBasicFileLoggerEngine(qDir.absoluteFilePath(QString("Kinect_%1.log").arg(nowDate)));
	// qxtLog takes ownership of dbg - no need to manage its memory
//	qxtLog->addLoggerEngine("dbg", dbg);
	qxtLog->addLoggerEngine("app", info);
	qxtLog->disableAllLogLevels();

//	qxtLog->enableLogLevels("dbg", QxtLogger::AllLevels);
	qxtLog->enableLogLevels("app",  QxtLogger::InfoLevel | QxtLogger::WarningLevel |
													QxtLogger::ErrorLevel | QxtLogger::FatalLevel);

	qxtLog->info("Monitor Started");
}










