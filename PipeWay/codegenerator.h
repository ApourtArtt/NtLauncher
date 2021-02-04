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
