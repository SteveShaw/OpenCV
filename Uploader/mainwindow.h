#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QFileSystemWatcher>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void SetMQContext(void* ctx)
	{
		m_MQCtx = ctx;
	}

	void CreateMQSock();

//	void SetupWatcher(const char *path);

signals:
		void stop_signal();

private:
	Ui::MainWindow *ui;
	void *m_MQCtx;
	void *m_MQSock;

	bool m_PauseRecord;
//	QFileSystemWatcher* m_Watcher;

//public slots:
//	void timer_signaled();
//	void dir_change_signaled(const QString& path);
private slots:
	void on_buttonSetPath_clicked();

	void OnRecordStarted();
	void OnTransformSignal(QString msg);
	void on_btnStartStop_clicked();
};

#endif // MAINWINDOW_H
