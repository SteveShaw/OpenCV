#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QRunnable>
#include <QObject>
#include <QTimer>
#include <QEventLoop>
#include <QList>
#include <QString>
#include <QMutex>

class FileWatcher : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit FileWatcher(QObject* parent = 0);
	~FileWatcher();

protected:
	void run();

public slots:
	void timer_job();
	void latest_file(QString path);

private:
	QEventLoop *m_evtLoop;
	QTimer *m_timer;

	QMutex m_mutex;

	QList<QString> m_filePathList;
};

#endif // FILEWATCHER_H
