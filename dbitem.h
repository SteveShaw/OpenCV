#ifndef DBITEM_H
#define DBITEM_H

#include <QString>
#include <QByteArray>

struct DBItem
{
	QString file_path;
	QString start;
	QString end;
	QString dir;
	QString type;

	QByteArray row;

	void ComposeRow()
	{
		row = start.toStdString().c_str();
		row += ";";
		row += end;
		row += ";";
		row += file_path;
		row += ";";
		row += dir;
		row += ";";
		row += type;
	}
};


#endif // DBITEM_H

