#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "networkrequester.h"

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    CodeGenerator(QObject *parent = nullptr);
    QString connectToAccount(QString username, QString password, QString gfuid = nullptr);
    QString getGfuid();

signals:
    void codeRecieved(QString code);

private:
    void retrieveEmailAddress();
    void connectWithEmail();
    void retrieveCode();
    QString generateGfuid();
    bool isValidGfuid(QString gfuid);

    NetworkRequester netRequester;
    QString token;
    QString platformGameAccountID;
    QString email;
    QString code;
    QString currentUsername, currentPassword, currentGfuid;

    const QString PLATFORMGAMEID = "dd4e22d6-00d1-44b9-8126-d8b40e0cd7c9";
    const QString POSSIBLE_CHARACTER = "0123456789abcdef";
    const QString LOCALE = "fr_FR";
    const QString GFLANG = "fr";
};

#endif // CODEGENERATOR_H
