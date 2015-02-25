#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QThreadPool>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
//	m_Watcher(new QFileSystemWatcher(parent))
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	emit stop_signal();
	QThreadPool::globalInstance()->waitForDone();

	delete ui;
//	delete m_Watcher;
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

void MainWindow::on_buttonStartStop_clicked()
{
	//ui->buttonStartStop->setText("Stop");
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
