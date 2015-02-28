#ifndef SINGLEAPP_H
#define SINGLEAPP_H

#include <QCoreApplication>
#include <QSharedMemory>


class SingleApplication : public QCoreApplication
{
	Q_OBJECT
public:
	SingleApplication(int &argc, char *argv[], const QString uniqueKey);

	bool isRunning();
	bool sendMessage(const QString &message);

public slots:
	void checkForMessage();

signals:
	void messageAvailable(QString message);

private:
	bool _isRunning;
	QSharedMemory sharedMemory;
};

#endif // SINGLEAPP_H
