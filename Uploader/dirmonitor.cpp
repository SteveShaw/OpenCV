#include "dirmonitor.h"

DirMonitor::DirMonitor(QObject *parent)
	:m_Watcher(new QFileSystemWatcher(parent))
{
}

DirMonitor::~DirMonitor()
{

}

void DirMonitor::AddMonitorDirectory(QString path)
{
	m_Watcher->addPath(path);
}

void DirMonitor::SetupWatcher()
{
	connect(m_Watcher,SIGNAL(directoryChanged(QString)),this,SLOT(directory_changed(QString)));
}

void DirMonitor::directory_changed(const QString &path)
{

}

