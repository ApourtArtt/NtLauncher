#ifndef CODECREATOR_H
#define CODECREATOR_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QString>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include "networkrequester.h"
#include "rc4.h"

#include <QDebug>
#include <QSslSocket>

class CodeCreator
{
public:
    CodeCreator(QString PlatformGameAccountId, QString InstallationId, QString ChromeVersion, QString CertificatePath)
        : gfuid(InstallationId)
        , platformgaid(PlatformGameAccountId)
        , chromeVersion(ChromeVersion)
        , certificatePath(CertificatePath)
    {
        qDebug() << gfuid << platformgaid << chromeVersion << certificatePath;
    }

    QString CreateCode()
    {
        QByteArray EncryptionKey = generateEncryptionKey();//"edd76c5219499d00da2c5a9e2b703c03d5aaf67d1f16c7ecd3165869921148f7";

        if (IsFirstDigitEven(gfuid))
        {
            QByteArray code = sha256(EncryptionKey + sha1("C"+chromeVersion.toLatin1()) + sha256(gfuid.toLatin1()) + sha1(platformgaid.toLatin1()));
            return platformgaid.left(2) + code.left(8);
        }
        QByteArray code = sha256(EncryptionKey + sha256("C"+chromeVersion.toLatin1()) + sha1(gfuid.toLatin1()) + sha256(platformgaid.toLatin1()));
        return platformgaid.left(2) + code.right(8);
    }

    bool ValidateEndpoint(QString clientSessionId)
    {
        QByteArray json = "{"
                          "\"account_id\":\"" + platformgaid.toLatin1() + "\","
                          "\"client_installation_id\":\"" + gfuid.toLatin1() + "\","
                          "\"client_session_id\":\"" + clientSessionId.toLatin1() + "\","
                          "\"id\":1,"
                          "\"start_count\":1,"
                          "\"start_time\":7000,"
                          "\"type\":\"start_time\""
                          "}";
        NetworkRequester netRequester;
        QNetworkRequest req(QUrl("https://events2.gameforge.com"));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        req.setRawHeader("User-Agent", "GameforgeClient/" + chromeVersion.left(chromeVersion.lastIndexOf('.')).toLatin1());
        req.setRawHeader("Content-Length", QString(json.length()).toLatin1());
        QByteArray response = netRequester.post(json, req);
        qDebug() << response;
        qDebug() << json;
        return response == "ok";
    }

private:
    QString gfuid;          // InstallationID => HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Gameforge4d\GameforgeClient\MainApp\InstallationId
    QString platformgaid;   // PlatformGameAccountID => From request to
    QString chromeVersion;  // Present after the "Chrome/" in the user agent
    QString certificatePath;// Path to the .p12 certificate

    const QByteArray RC4KEY = "mxSJQu496fFHTYyA";
    const QByteArray RC4BODY = "OWxRfc8KTL8IvkbUzutMgg==";

    QByteArray getCertificate(QByteArray key)
    {
        qDebug() << QSslSocket::supportsSsl();
        QFile file(certificatePath);
        qDebug() << "Key: " << key;
        if (!file.open(QIODevice::ReadOnly))
            return "";
        QSslKey sslKey;
        QSslCertificate cert;
        QList<QSslCertificate> certs;
        if (!QSslCertificate::importPkcs12(&file, &sslKey, &cert, &certs, key))
            return "";
        qDebug() << "Import successful";
        return cert.toPem();
    }

    QByteArray generatePassword()
    {
        unsigned char* cipher = (unsigned char*)malloc(257);
        for(int i = 0 ; i < 257 ; i++) cipher[i] = 0;

        QByteArray key = QByteArray(RC4KEY);
        QByteArray encryptionKey = QByteArray::fromBase64(RC4BODY);
        RC4(key, encryptionKey, cipher);

        QByteArray password = QByteArray((char*)cipher);

        return password;
    }

    QByteArray generateEncryptionKey()
    {
        QByteArray sslPassword = generatePassword();
        QByteArray pem = getCertificate(sslPassword);

        if (pem.isEmpty())
            return QByteArray();

        if (IsFirstDigitEven(gfuid))
            return sha256(pem);
        return sha1(pem);
    }

    QByteArray sha256(QByteArray input)
    {
        QCryptographicHash crypt(QCryptographicHash::Algorithm::Sha256);
        crypt.addData(input, input.size());
        return crypt.result().toHex();
    }

    QByteArray sha1(QByteArray input)
    {
        QCryptographicHash crypt(QCryptographicHash::Algorithm::Sha1);
        crypt.addData(input, input.size());
        return crypt.result().toHex();
    }

    bool IsFirstDigitEven(QString InstallationId)
    {
        for (int i = 0 ; i < InstallationId.size() ; i++)
        {
            if (InstallationId[i].isDigit())
                return (InstallationId[i].digitValue() % 2 == 0);
        }
        return true;
    }
};

#endif // CODECREATOR_H
