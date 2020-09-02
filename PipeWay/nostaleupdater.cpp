#include "nostaleupdater.h"

NostaleUpdater::NostaleUpdater(QObject *parent)
    : QObject(parent)
{
    updated = false;
}

void NostaleUpdater::changeFolder(QDir NostaleFolder)
{
    nostaleFolder = NostaleFolder;
}

bool NostaleUpdater::checkForUpdate()
{
    if(updated)
    {
        emit resultUpdated(updated);
        return true;
    }
    generateRemoteNostaleFiles();
    updated = generateLocalNostaleFiles();
    qDebug() << ntRemoteFiles.size() << ntLocalFiles.size();
    for(int i = 0 ; i < ntRemoteFiles.size() ; i++)
    {
        if(ntRemoteFiles[i].isFolder())
            continue;
        for(int j = 0 ; j < ntLocalFiles.size() ; j++)
        {
            if(ntLocalFiles[j].getFile() == ntRemoteFiles[i].getFile())
            {
                qDebug() << ntRemoteFiles[i].getFile() << " : " << ntRemoteFiles[i].getSha1() << ntLocalFiles[j].getSha1();
                if(ntLocalFiles[j].getSha1() != ntRemoteFiles[i].getSha1())
                {
                    updated = false;
                    if(!diffFiles.contains(ntRemoteFiles[i]))
                        diffFiles.push_back(ntRemoteFiles[i]);
                }
            }
        }
    }
    emit resultUpdated(updated);
    return updated;
}

bool NostaleUpdater::update()
{
    for(int i = 0 ; i < diffFiles.size() ; i++)
    {
        if(!downloadAndSave(diffFiles[i]))
            return false;
    }
    return true;
}

bool NostaleUpdater::repair()
{
    for(int i = 0 ; i < ntRemoteFiles.size() ; i++)
    {
        if(!downloadAndSave(ntRemoteFiles[i]))
            return false;
    }
    return true;
}

bool NostaleUpdater::isUpdated()
{
    return updated;
}

bool NostaleUpdater::generateLocalNostaleFiles()
{
    ntLocalFiles.clear();
    QStringList remoteFilenames = getRemoteFilenameList(true);
    QStringList localFilenames;
    QDirIterator it(nostaleFolder.path(), remoteFilenames, QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString sha1 = QString(fileChecksum(filename, QCryptographicHash::Sha1));
        filename.remove(nostaleFolder.path() + "/").replace("/", "\\");
        localFilenames.push_back(filename);

        ntLocalFiles.push_back(NtFile(filename, sha1));
    }


    bool similar = true;
    remoteFilenames = getRemoteFilenameList(false);
    for(int i = 0 ; i < remoteFilenames.size() ; i++)
    {
        if(!localFilenames.contains(remoteFilenames[i]))
        {
            for(int j = 0 ; j < ntRemoteFiles.size() ; j++)
            {
                if(ntRemoteFiles[j].getFile() == remoteFilenames[i])
                {
                    diffFiles.push_back(ntRemoteFiles[j]);
                    similar = false;
                }
            }
        }
    }
    return similar;
}

void NostaleUpdater::generateRemoteNostaleFiles()
{
    ntRemoteFiles.clear();
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/patching/download/latest/nostale/default?locale=fr&architecture=x64&branchToken"));

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray response = netRequester.get(req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    QJsonValue jsonVal = jsonObj.value("entries");
    for(int i = 0 ; !jsonVal[i].isUndefined() ; i++)
    {
        QJsonObject curr = jsonVal[i].toObject();
        ntRemoteFiles.push_back(NtFile(curr["path"].toString(), curr["sha1"].toString(), curr["file"].toString(),
                static_cast<short>(curr["flags"].toInt()), static_cast<size_t>(curr["size"].toInt()),
                curr["folder"].toBool()));
    }
}

bool NostaleUpdater::downloadAndSave(NtFile file)
{
    QNetworkRequest req(QUrl("http://patches.gameforge.com" + file.getPath()));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray response = netRequester.get(req);
    QFile ofile(nostaleFolder.path() + "/" + file.getFile());
    QDataStream inO(&ofile);
    if(!ofile.open(QIODevice::WriteOnly))
        return false;
    inO.writeRawData(response, response.size());
    ofile.close();
    return true;
}

QStringList NostaleUpdater::getRemoteFilenameList(bool split)
{
    QStringList filenames;
    for(int i = 0 ; i < ntRemoteFiles.size() ; i++)
    {
        if(!ntRemoteFiles[i].isFolder())
        {
            if(split)
            {
                QStringList pathFile = ntRemoteFiles[i].getFile().split("\\");
                filenames.push_back(pathFile[pathFile.size() - 1]);
            }
            else
                filenames.push_back(ntRemoteFiles[i].getFile());
        }
    }
    return filenames;
}

QByteArray NostaleUpdater::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f))
        {
            return hash.result().toHex();
        }
    }
    return QByteArray();
}



NtFile::NtFile(QString File, QString Sha1)
{
    path = "";
    sha1 = Sha1;
    file = File;
    flags = 0;
    size = 0;
    folder = false;
}

NtFile::NtFile(QString Path, QString Sha1, QString File, short Flags, size_t Size, bool Folder)
{
    path = Path;
    sha1 = Sha1;
    file = File;
    flags = Flags;
    size = Size;
    folder = Folder;
}

QString NtFile::getPath() const
{
    return path;
}

QString NtFile::getSha1() const
{
    return sha1;
}

QString NtFile::getFile() const
{
    return file;
}

short NtFile::getFlags() const
{
    return flags;
}

size_t NtFile::getSize() const
{
    return size;
}

bool NtFile::isFolder() const
{
    return folder;
}
