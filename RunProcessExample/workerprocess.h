#ifndef WORKERPROCESS_H
#define WORKERPROCESS_H

#include <QRunnable>
#include <QObject>
#include <QProcess>
#include <QMetaType>

class WorkerProcess : public QObject, public QRunnable
{
  Q_OBJECT
  Q_ENUMS(QProcess::ExitStatus)
  Q_ENUMS(QProcess::ProcessError)

public:
  explicit WorkerProcess(QObject* parent=0);
  virtual ~WorkerProcess(){}

  void SetupWorker();

protected:
  void run();

protected slots:
  void OnFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void OnStarted();
  void OnError(QProcess::ProcessError error);
  void OnOutputReady();

signals:
  void TransformSignal(QString);
};



#endif // WORKERPROCESS_H
