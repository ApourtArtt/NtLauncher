#ifndef NOSTALEUPDATER_H
#define NOSTALEUPDATER_H

#include "networkrequester.h"
#include <QJsonDocument>
#include <QDirIterator>

class NtFile
{
public:
    NtFile(QString File, QString Sha1);
    NtFile(QString Path, QString Sha1, QString File, short Flags, size_t Size, bool Folder);

    QString getPath() const;
    QString getSha1() const;
    QString getFile() const;
    short getFlags() const;
    size_t getSize() const;
    bool isFolder() const;

    bool operator==(const NtFile &otherFile) const
    {
        return sha1 == otherFile.sha1 && file == otherFile.file;
    }

private:
    QString path;
    QString sha1;
    QString file;
    short flags;
    size_t size;
    bool folder;
};

class NostaleUpdater : public QObject
{
    Q_OBJECT
public:
    NostaleUpdater(QObject *parent);
    void changeFolder(QDir NostaleFolder);
    bool checkForUpdate();
    bool update();
    bool repair();
    bool isUpdated();

signals:
    void resultUpdated(bool updated);

private:
    bool generateLocalNostaleFiles();
    void generateRemoteNostaleFiles();

    bool downloadAndSave(NtFile file);
    QStringList getRemoteFilenameList(bool split);
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);

    NetworkRequester netRequester;
    QDir nostaleFolder;
    QVector<NtFile> ntRemoteFiles, ntLocalFiles, diffFiles;
    bool updated;
};

#endif // NOSTALEUPDATER_H
