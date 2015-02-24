#ifndef DBITEM_H
#define DBITEM_H

#include <QString>
#include <QByteArray>

struct DBItem
{
	QString colorFileName;
	QString depthFileName;
	QString start;
	QString end;
	QString dir;

	QByteArray row;

	void ComposeRow()
	{
		row = start.toStdString().c_str();
		row += ";";
		row += end;
		row += ";";
		row += dir;
		row += ";";
		row += colorFileName;
		row += ";";
		row += depthFileName;
	}
};

struct DBConfig
{
	QString host;
	QString user;
	QString pwd;
	QString db;	//database name
	QString tbl;//table
};

struct FTPConfig
{
	QString host;
	QString user;
	QString pwd;
	QString path;
};



#endif // DBITEM_H

