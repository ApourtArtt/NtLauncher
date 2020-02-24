#include "filepatching.h"

FilePatching::FilePatching(QString dir)
{
    pGfSkipper = "\xe9????\xa1????\x80\x38\x01\x0f\x85????\xa1????\x8a?\x34\x01\xe8????\x84\xc0\x75\x20\x6a";
    nGfSkipper = "\xe9????\xa1????\x80\x38\x01\x0f\x85????\xa1????\x8a?\x34\x01\xe9\xb3";
    nGfSkipper.append('\x00'); nGfSkipper.append('\x00'); nGfSkipper.append('\x00');
    nGfSkipper.append("\x84\xc0\x75\x20\x6a");
    QStringList tmp = dir.split("/");
    tmp.removeLast();
    directory = tmp.join("/");
}

FilePatching::~FilePatching()
{
    
}

bool FilePatching::patchFile()
{
    if (QFile::exists(directory + "/" + PATCHED_FILENAME))
    {
        if (getFileVersion(directory + "/" + PATCHED_FILENAME) != getFileVersion(directory + "/" + ORIGINAL_FILENAME))
            return modify();
        return true;
    }
    return modify();
}

QString FilePatching::getPatchedFilename()
{
    return directory + "/" + PATCHED_FILENAME;
}

void FilePatching::changeDir(QString dir)
{
    QStringList tmp = dir.split("/");
    tmp.removeLast();
    directory = tmp.join("/");
}

QString FilePatching::getFileVersion(QString filename)
{
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(filename.toStdWString().c_str(), &dwHandle);
    LPVOID lpData = new BYTE[dwLen];
    if (!GetFileVersionInfo(filename.toStdWString().c_str(), dwHandle, dwLen, lpData))
    {
        // delete[] lpData;
        return nullptr;
    }
    VS_FIXEDFILEINFO* lpBuffer = nullptr;
    UINT uLen;
    VerQueryValue(lpData, QString("\\").toStdWString().c_str(), (LPVOID*)&lpBuffer, &uLen);
    return  QString::number((lpBuffer->dwFileVersionMS >> 16)   & 0xffff) + "." +
            QString::number((lpBuffer->dwFileVersionMS)         & 0xffff) + "." +
            QString::number((lpBuffer->dwFileVersionLS >> 16)   & 0xffff) + "." +
            QString::number((lpBuffer->dwFileVersionLS)         & 0xffff);
}

bool FilePatching::modify()
{
    QFile ofile(directory + "/" + ORIGINAL_FILENAME);
    QDataStream inO(&ofile);
    if (!ofile.open(QIODevice::ReadOnly))
        return false;
    QByteArray data = ofile.read(ofile.size());
    ofile.close();

    QFile nfile(directory + "/" + PATCHED_FILENAME);
    QDataStream inN(&nfile);
    if (!nfile.open(QIODevice::WriteOnly))
        return false;

    data = parseOption(pGfSkipper, nGfSkipper, data);

    inN.writeRawData(data, data.size());
    nfile.close();
    return true;
}

QByteArray FilePatching::parseOption(QByteArray pattern, QByteArray newData, QByteArray data)
{
    int firstBit = getBitToBegin(pattern, data);
    if (firstBit == -1)
        return data;
    return patch(firstBit, newData, data);
}

int FilePatching::getBitToBegin(QByteArray pattern, QByteArray data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] == pattern[0])
        {
            int j = 0;
            bool equal = true;
            for (; j < pattern.size(); j++)
            {
                if (pattern[j] != data[i + j] && pattern[j] != '?')
                {
                    equal = false;
                    break;
                }
            }
            if (equal && j > 0)
                return i;
        }
    }
    return -1;
}

QByteArray FilePatching::patch(int firstBit, QByteArray newData, QByteArray data)
{
    for (int i = 0; i < newData.size(); i++)
    {
        if (newData[i] != '?')
            data[i + firstBit] = newData[i];
    }
    return data;
}
