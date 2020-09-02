#include "codegenerator.h"

QString CodeGenerator::gfClientVersion = nullptr;

CodeGenerator::CodeGenerator(QString PlatformGameId, QObject *parent)
    : QObject(parent)
    , netRequester(this)
    , platformGameId(PlatformGameId)
{
    if(gfClientVersion == nullptr)
        gfClientVersion = getGfClientVersion();
}

QString CodeGenerator::connectToAccount(QString username, QString password, QString lang, QString gfuid)
{
    currentUsername = username;
    currentPassword = password;
    local = langToParam.value(lang).first;
    gflang = lang.toLower();
    currentGfuid = gfuid.isEmpty() ? getGfuidFromRegistry() : gfuid;
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
    QSettings settings("HKEY_CURRENT_USER\\Software\\Gameforge4d\\TNTClient\\MainApp", QSettings::NativeFormat);
    return settings.value("InstallationId", "").toString();
}

bool CodeGenerator::retrieveEmailAddress()
{
    QByteArray json = "{"
                      "\"identity\":\"" + currentUsername.toUtf8() + "\","
                      "\"password\":\"" + currentPassword.toUtf8() + "\","
                      "\"locale\":\"" + local.toUtf8() + "\","
                      "\"gfLang\":\"" + gflang.toUtf8() + "\","
                      "\"platformGameId\":\"" + platformGameId.toUtf8() + "\""
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
                      "\"platformGameId\":\"" + platformGameId.toUtf8() + "\""
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
    qDebug() << response;
    return (!token.isNull());
}

QString CodeGenerator::retrieveCode()
{
    QByteArray json = "{"
                      "\"platformGameAccountId\":\"" + platformGameAccountID.toUtf8() + "\""
                      "}";
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/auth/thin/codes"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("TNT-Installation-Id", currentGfuid.toUtf8());
    req.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    req.setRawHeader("User-Agent", gfClientVersion.toUtf8());
    req.setRawHeader("Content-Length", QString(json.length()).toUtf8());
    req.setRawHeader("Connection", "Keep-Alive");
    req.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    QByteArray response = netRequester.post(json, req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    code = jsonObj.value("code").toString().toLatin1();
    qDebug() << response;
    return code;
}

QString CodeGenerator::getGfClientVersion()
{
    QNetworkRequest req(QUrl("http://dl.tnt.gameforge.com/tnt/final-ms3/clientversioninfo.json"));
    QByteArray response = netRequester.get(req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    QString version = jsonObj.value(QString("minimumVersionForDelayedUpdate")).toString();
    qDebug() << "CodeGenerator::getGfClientVersion() : " << version;
    if(version == nullptr)
        return "GameforgeClient/2.1.5";
    return "GameforgeClient/" + version;
}
