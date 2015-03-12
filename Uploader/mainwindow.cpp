#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QThreadPool>
#include <QDir>
#include <zmq.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_MQCtx(NULL),
	m_MQSock(NULL),
	m_PauseRecord(false)
//	m_Watcher(new QFileSystemWatcher(parent))
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	emit stop_signal();
	QThreadPool::globalInstance()->waitForDone();

	if(m_MQSock)
	{
		zmq_close(m_MQSock);
	}
	delete ui;
	//	delete m_Watcher;
}

void MainWindow::CreateMQSock()
{
	m_MQSock = zmq_socket(m_MQCtx,ZMQ_PUB);
	zmq_bind(m_MQSock,"tcp://*:9988");
}

//void MainWindow::SetupWatcher(const char* path)
//{
//	m_Watcher->addPath(path);
//	connect(m_Watcher,SIGNAL(directoryChanged(QString)),this,SLOT(dir_change_signaled(QString)));
//}

//void MainWindow::timer_signaled()
//{
//	ui->textEdit->append(QDateTime::currentDateTime().toString());
//}

//void MainWindow::dir_change_signaled(const QString &path)
//{
//	QDir qDir(path);
//	QStringList filters;
//	filters.append("*.m4v");
//	QStringList fileList = qDir.entryList(filters,QDir::Files,QDir::Time|QDir::Name);

//	foreach(const QString &path, fileList)
//	{
//		ui->textEdit->append(path);
//	}
//}

void MainWindow::on_buttonSetPath_clicked()
{

}

void MainWindow::OnRecordStarted()
{
	QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd:HH-mm-ss:zzz");
	ui->textEdit->append(now+"---Record Started");
}

void MainWindow::OnTransformSignal(QString msg)
{
	ui->textEdit->append(msg);
}

void MainWindow::on_btnStartStop_clicked()
{
	m_PauseRecord = !m_PauseRecord;
	if(m_PauseRecord)
	{
		zmq_send(m_MQSock,"PAU",3,0);
		ui->btnStartStop->setText("Resume");
	}
	else
	{
		zmq_send(m_MQSock,"RES",3,0);
		ui->btnStartStop->setText("Pause");
	}

}
