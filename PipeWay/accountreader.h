#ifndef ACCOUNTREADER_H
#define ACCOUNTREADER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDir>

class Account
{
public:
    Account();
    Account(QString DisplayedName, QString Username, QString Password, QString Lang, QString Gfuid, QString Cache);

    QString getDisplayedName() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getLang() const;
    QString getGfuid() const;
    QString getCache() const;

    void setDisplayedName(QString DisplayedName);
    void setUsername(QString Username);
    void setPassword(QString Password);
    void setLang(QString Lang);
    void setGfuid(QString Gfuid);
    void setCache(QString Cache);

private:
    QString displayedName;
    QString username;
    QString password;
    QString lang;
    QString gfuid;
    QString cache;
};

class AccountReader : public QObject
{
    Q_OBJECT
public:
    AccountReader(QObject *parent);
    bool changeAccountFile(QString NewAccountFilename);
    bool addAccount(QString displayedName, QString username, QString password, QString lang, QString gfuid, QString cache, int position, QString filename);
    bool addAccount(const Account account, int position);
    bool deleteAccount(size_t position);
    bool patchAccountCache(QString displayedName, QString cache);
    bool addAccountFile(QString filename);
    QStringList getAccountFileList();
    const std::vector<Account> getAccounts();

signals:
    //void accountFileChanged(QString filename);

private:
    bool createAccountFile();
    bool readAccountFile();
    QPair<size_t, Account> getAccountData(QString displayedName);

    std::vector<Account> currentAccountList;
    QStringList accountFileList;
    QString currentAccountFile;
};

#endif // ACCOUNTREADER_H
