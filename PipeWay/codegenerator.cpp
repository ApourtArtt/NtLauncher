#include "codegenerator.h"
#include "codecreator.h"


CodeGenerator::CodeGenerator(QString PlatformGameId, QObject *parent)
    : QObject(parent)
    , netRequester(this)
    , platformGameId(PlatformGameId)
{

}

QString CodeGenerator::connectToAccount(QString username, QString password, QString lang, QString gfuid)
{
    currentUsername = username;
    currentPassword = password;
    local = langToParam.value(lang).first;
    gflang = lang.toLower();
    currentGfuid = gfuid.isEmpty() ? getGfuidFromRegistry() : gfuid;
    qDebug() << "Gfuid : " << currentGfuid;
    if(!retrieveEmailAddress())
        return nullptr;
    if(!connectWithEmail())
        return nullptr;
    return retrieveCode();
}

QString CodeGenerator::getGfuid()
{
    return currentGfuid;
}

QString CodeGenerator::getGfuidFromRegistry()
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Gameforge4d\\GameforgeClient\\MainApp", QSettings::NativeFormat);
    return settings.value("InstallationId", "").toString();
}

bool CodeGenerator::retrieveEmailAddress()
{
    QByteArray json = "{"
                      "\"identity\":\"" + currentUsername.toUtf8() + "\","
                      "\"password\":\"" + currentPassword.toUtf8() + "\","
                      "\"locale\":\"" + local.toUtf8() + "\","
                      "\"gfLang\":\"" + gflang.toUtf8() + "\","
                      "\"platformGameId\":\"" + platformGameId.toUtf8() + "\","
                      "\"gameEnvironmentId\":\"732876de-012f-4e8d-a501-2e0816cf22f2\","
                      "\"autoGameAccountCreation\":true"
                      "}";
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/auth/thin/sessions"));
    req.setRawHeader("Connection", "Keep-Alive");
    req.setRawHeader("Content-Length", QString(json.length()).toUtf8());
    req.setRawHeader("Origin", "spark://www.gameforge.com");
    req.setRawHeader("TNT-Installation-Id", currentGfuid.toUtf8());
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "*/*");
    req.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    QByteArray response = netRequester.post(json, req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    token = jsonObj.value(QString("token")).toString();
    platformGameAccountID = jsonObj.value(QString("platformGameAccountId")).toString();
    email = jsonObj.value(QString("email")).toString();
    qDebug() << json;
    qDebug() << response;
    return (!email.isNull() && !token.isNull() && !platformGameId.isNull());
}

bool CodeGenerator::connectWithEmail()
{
    QByteArray json = "{"
                      "\"identity\":\"" + email.toUtf8() + "\","
                      "\"password\":\"" + currentPassword.toUtf8() + "\","
                      "\"locale\":\"" + local.toUtf8() + "\","
                      "\"gfLang\":\"" + gflang.toUtf8() + "\","
                      "\"platformGameId\":\"" + platformGameId.toUtf8() + "\","
                      "\"autoGameAccountCreation\":true"
                      "}";
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/auth/thin/sessions"));
    req.setRawHeader("Connection", "Keep-Alive");
    req.setRawHeader("Content-Length", QString(json.length()).toUtf8());
    req.setRawHeader("Origin", "https://gameforge.com");
    req.setRawHeader("TNT-Installation-Id", currentGfuid.toUtf8());
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("Accept", "*/*");
    req.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    QByteArray response = netRequester.post(json, req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    token = jsonObj.value(QString("token")).toString();
    QString valuePlatform = jsonObj.value(QString("platformGameAccountId")).toString();
    qDebug() << json;
    qDebug() << response;
    return (!token.isNull());
}

QString CodeGenerator::retrieveCode()
{
    QByteArray json = "{"
                      "\"platformGameAccountId\":\"" + platformGameAccountID.toUtf8() + "\""
                      "}";
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/auth/thin/codes"));
    req.setRawHeader("TNT-Installation-Id", currentGfuid.toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    req.setRawHeader("User-Agent", getUserAgent());
    req.setRawHeader("Content-Length", QString(json.length()).toUtf8());
    req.setRawHeader("Connection", "Keep-Alive");
    req.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    QByteArray response = netRequester.post(json, req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    code = jsonObj.value("code").toString().toLatin1();
    qDebug() << json;
    qDebug() << response;
    for (int i = 0; i < req.rawHeaderList().size() ; i++)
    {
        qDebug() << "- " << req.rawHeaderList()[i] << " : " << req.rawHeader(req.rawHeaderList()[i]);
    }
    return code;
}

QByteArray CodeGenerator::getUserAgent()
{
    QString userAgent = "Chrome/C";
    userAgent.append("2.1.22.784"); // todo
    userAgent.append(" (");

    CodeCreator cc(platformGameAccountID, currentGfuid, "2.1.22.784", "cert.p12");
    cc.ValidateEndpoint(generateRandomUuid());
    userAgent.append(cc.CreateCode());

    userAgent.append(") ");
    userAgent.append("GameforgeClient/2.1.22"); // todo

    qDebug() << userAgent;
    return userAgent.toLatin1();
}

QString CodeGenerator::generateRandomUuid()
{
    const int STRING_LENGTH = 36;
    const QString POSSIBLE_CHARACTER = "0123456789abcdef";
    QString randomString;
    randomString.reserve(STRING_LENGTH);
    for(int i = 0; i < STRING_LENGTH; i++)
    {
        int index = qrand() % POSSIBLE_CHARACTER.length();
        QChar nextChar = POSSIBLE_CHARACTER.at(index);
        if(i == 14)
            nextChar = '4';
        if(i == 8 || i == 13 || i == 18 || i == 23)
            nextChar = '-';
        randomString.append(nextChar);
    }
    return randomString;
}
