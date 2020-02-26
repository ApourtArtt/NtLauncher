#include "codegenerator.h"

CodeGenerator::CodeGenerator(QObject *parent)
    : QObject(parent)
    , netRequester(this)
{
    code = nullptr;
}

QString CodeGenerator::connectToAccount(QString username, QString password, QString lang, QString gfuid)
{
    currentUsername = username;
    currentPassword = password;
    local = langToLocale.value(lang);
    gflang = lang.toLower();
    currentGfuid = gfuid;
    if(!(isValidGfuid(currentGfuid)))
        currentGfuid = generateGfuid();
    retrieveEmailAddress();
    connectWithEmail();
    retrieveCode();
    return code;
}

QString CodeGenerator::getGfuid()
{
    return currentGfuid;
}

void CodeGenerator::retrieveEmailAddress()
{
    QByteArray json = "{"
                      "\"identity\":\"" + currentUsername.toUtf8() + "\","
                      "\"password\":\"" + currentPassword.toUtf8() + "\","
                      "\"locale\":\"" + local.toUtf8() + "\","
                      "\"gfLang\":\"" + gflang.toUtf8() + "\","
                      "\"platformGameId\":\"" + PLATFORMGAMEID.toUtf8() + "\""
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
}

void CodeGenerator::connectWithEmail()
{
    QByteArray json = "{"
                      "\"identity\":\"" + email.toUtf8() + "\","
                      "\"password\":\"" + currentPassword.toUtf8() + "\","
                      "\"locale\":\"" + local.toUtf8() + "\","
                      "\"gfLang\":\"" + gflang.toUtf8() + "\","
                      "\"platformGameId\":\"" + PLATFORMGAMEID.toUtf8() + "\""
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
}

void CodeGenerator::retrieveCode()
{
    QByteArray json = "{"
           "\"platformGameAccountId\":\"" + platformGameAccountID.toUtf8() + "\""
           "}";
    QNetworkRequest req(QUrl("https://spark.gameforge.com/api/v1/auth/thin/codes"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("TNT-Installation-Id", currentGfuid.toUtf8());
    req.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    req.setRawHeader("User-Agent", "GameforgeClient/2.0.50");
    req.setRawHeader("Content-Length", QString(json.length()).toUtf8());
    req.setRawHeader("Connection", "Keep-Alive");
    req.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    QByteArray response = netRequester.post(json, req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    code = jsonObj.value("code").toString().toLatin1();
    codeRecieved(code);
}

QString CodeGenerator::generateGfuid()
{
    const int STRING_LENGTH = 36;
    QString randomString;
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

bool CodeGenerator::isValidGfuid(QString gfuid)
{
    if(gfuid == nullptr)
        return false;
    if(gfuid.size() != 36)
        return false;
    for(int i = 0 ; i < gfuid.size() ; i++)
    {
        switch(i)
        {
        case 8: case 13: case 18: case 23:
            if(gfuid.at(i) != '-')
                return false;
            break;
        case 14:
            if(gfuid.at(i) != '4')
                return false;
            break;
        default:
            if(!POSSIBLE_CHARACTER.contains(gfuid.at(i)))
                return false;
            break;
        }
    }
    return true;
}
