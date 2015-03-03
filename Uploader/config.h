#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

struct AppConfig
{
	QString videoPath;
	int roomID;
	int kinectID;
};

struct FTPConfig
{
	QString host;
	QString user;
	QString pwd;
	QString path;
};

struct DBConfig
{
	QString host;
	QString user;
	QString pwd;
	QString tbl;
	QString db;
};

#endif // CONFIG_H

