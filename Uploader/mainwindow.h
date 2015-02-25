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

//	void SetupWatcher(const char *path);

signals:
		void stop_signal();

private:
	Ui::MainWindow *ui;
//	QFileSystemWatcher* m_Watcher;

//public slots:
//	void timer_signaled();
//	void dir_change_signaled(const QString& path);
private slots:
	void on_buttonSetPath_clicked();
	void on_buttonStartStop_clicked();

	void OnRecordStarted();
	void OnTransformSignal(QString msg);
};

#endif // MAINWINDOW_H
