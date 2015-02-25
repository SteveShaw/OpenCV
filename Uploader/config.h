#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

struct AppConfig
{
	QString videoPath;
};

struct FTPConfig
{
	QString host;
	QString user;
	QString pwd;
	QString path;
};

#endif // CONFIG_H
