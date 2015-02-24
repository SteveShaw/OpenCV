#include <QThreadPool>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workerprocess.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_buttonBox_accepted()
{
  WorkerProcess *worker = new WorkerProcess();
  worker->setAutoDelete(true);
  worker->SetupWorker();

  connect(worker,SIGNAL(TransformSignal(QString)),this,SLOT(OnTransformedMessage(QString)));

  QThreadPool::globalInstance()->start(worker,QThread::LowPriority);
}

void MainWindow::on_buttonBox_rejected()
{
  ui->textEdit->append("rejected");
}

void MainWindow::OnTransformedMessage(QString msg)
{
  ui->textEdit->append(msg);
}
