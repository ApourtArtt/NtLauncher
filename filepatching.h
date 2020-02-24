#ifndef FILEPATCHING_H
#define FILEPATCHING_H
#pragma once
#include <Windows.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qfile.h>
#include <QtCore/qdatastream.h>

class FilePatching
{
public:
	FilePatching(QString dir);
	~FilePatching();
	bool patchFile();
    QString getPatchedFilename();
	void changeDir(QString dir);

private:
	QString getFileVersion(QString filename);
	bool modify();
	QByteArray parseOption(QByteArray pattern, QByteArray newData, QByteArray data);
	int getBitToBegin(QByteArray pattern, QByteArray data);
	QByteArray patch(int firstBit, QByteArray newData, QByteArray data);

	QString directory;
	QByteArray pGfSkipper;
	QByteArray nGfSkipper;

	const QString PATCHED_FILENAME = "NostaleClientXPatched.exe";
	const QString ORIGINAL_FILENAME = "NostaleClientX.exe";
};

#endif
