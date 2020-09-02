#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "networkrequester.h"
#include <QMap>

const QMap <QString, QPair<QString, int>> langToParam
{
    // langToParam.value("EN").first;  : en_uk
    // langToParam.value("EN").second; : 0
    { "EN", { "en_UK", 0 } },
    { "DE", { "de_DE", 1 } },
    { "FR", { "fr_FR", 2 } },
    { "IT", { "it_IT", 3 } },
    { "PL", { "pl_PL", 4 } },
    { "ES", { "es_ES", 5 } },
    { "CZ", { "cz_CZ", 6 } },
    { "RU", { "ru_RU", 7 } },
    { "TR", { "tr_TR", 8 } }
};


/*
 * TODO : func to create account
POST https://spark.gameforge.com/api/v1/user/me HTTP/1.1
Host: spark.gameforge.com
Connection: keep-alive
Content-Length: 104
Origin: spark://www.gameforge.com
TNT-Installation-Id: 37123123-e2ee-46cb-8a70-9775ec6a6aab
User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36
Content-Type: application/json
Accept: * / *
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.9
{"email":"emailemail@gmail.com","password":"motdepasse123P","displayName":"emailemail","locale":"fr-FR"}

no need to activate account
password need 10 characters minimum, 1 maj, 1 min, 1 number minimum
*/

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    CodeGenerator(QString PlatformGameId, QObject *parent = nullptr);
    QString connectToAccount(QString username, QString password, QString lang, QString gfuid = nullptr);
    QString getGfuid();
    static QString getGfuidFromRegistry();

private:
    bool retrieveEmailAddress();
    bool connectWithEmail();
    QString retrieveCode();
    QString getGfClientVersion();

    NetworkRequester netRequester;

    QString token;
    QString platformGameAccountID;
    QString email;
    QString code;
    QString currentUsername, currentPassword, currentGfuid;
    QString local, gflang;

    QString platformGameId;
    static QString gfClientVersion;
};

#endif // CODEGENERATOR_H
