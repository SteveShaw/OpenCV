#ifndef DIRMONITOR_H
#define DIRMONITOR_H

#include <QFileSystemWatcher>

class DirMonitor : public QObject
{
	Q_OBJECT
public:
	explicit DirMonitor(QObject* parent=0);
	virtual ~DirMonitor();

	void AddMonitorDirectory(QString path);

	void SetupWatcher();

private:
	QFileSystemWatcher* m_Watcher;

public slots:
	void directory_changed(const QString& path);
};

#endif // DIRMONITOR_H
