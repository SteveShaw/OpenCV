#include "workerprocess.h"
#include <QProcess>
#include <QDebug>

WorkerProcess::WorkerProcess(QObject *parent)
  :QObject(parent)
{
}

void WorkerProcess::SetupWorker()
{
}


void WorkerProcess::run()
{

  QProcess* proc = new QProcess();
  QString program = "E:\\HB\\HBCLI.exe";
  QString inputVideoFile = "E:\\HB\\Test.wmv";
  QString outputVideoFile = "E:\\HB\\Test.mp4";
  QString opt = "--preset=\"Normal\"";

  qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
  qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
  connect(proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(OnFinished(int,QProcess::ExitStatus)));
  connect(proc,SIGNAL(started()),this,SLOT(OnStarted()));
  connect(proc,SIGNAL(error(QProcess::ProcessError)),this,SLOT(OnError(QProcess::ProcessError)));
  connect(proc,SIGNAL(readyReadStandardOutput()),this,SLOT(OnOutputReady()));

  QStringList arguments;
  arguments.append(QString(" -i %1 -o %2 %3").arg(inputVideoFile).arg(outputVideoFile).arg(opt));
  qDebug()<<arguments[0];
  program += arguments[0];
  proc->start(program);

  if(proc->waitForStarted())
  {
    proc->waitForFinished();
  }

  delete proc;
  proc = NULL;
}

void WorkerProcess::OnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if(exitStatus==QProcess::NormalExit)
  {
//    QString msg =
    emit TransformSignal(QString("Transform is completed:%1").arg(exitCode));
  }
  else
  {
    emit TransformSignal(QString("Transform is failed:"));
  }
}

void WorkerProcess::OnStarted()
{
  emit TransformSignal(QString("Transform is started"));
}

void WorkerProcess::OnError(QProcess::ProcessError error)
{
//  QProcess::FailedToStart	0	The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.
//  QProcess::Crashed	1	The process crashed some time after starting successfully.
//  QProcess::Timedout	2	The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.
//  QProcess::WriteError	4	An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.
//  QProcess::ReadError	3	An error occurred when attempting to read from the process. For example, the process may not be running.
//  QProcess::UnknownError	5	An unknown error occurred. This is the default return value of error().
  QString errMsg = "No Error";
  switch(error)
  {
  case QProcess::Crashed:
    errMsg = "Error: Crashed";
    break;
  case QProcess::FailedToStart:
    errMsg = "Error: Failed to Start";
    break;
  case QProcess::ReadError:
    errMsg = "Error: Read Error";
    break;
  case QProcess::WriteError:
    errMsg = "Error: Write Error";
    break;
  default:
    errMsg = "Error: Unknown Error";
    break;
  }

  emit TransformSignal(errMsg);
}

void WorkerProcess::OnOutputReady()
{
}


