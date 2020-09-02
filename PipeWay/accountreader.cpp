#include "accountreader.h"

AccountReader::AccountReader(QObject *parent)
    : QObject(parent)
{
    if(!QDir("accounts").exists())
        QDir().mkdir("accounts");
    QDir dir = QDir::currentPath() + "/accounts";
    accountFileList = dir.entryList({"*.acc"});
    if(accountFileList.size() > 0)
    {
        currentAccountFile = accountFileList[0];
        readAccountFile();
    }
}

bool AccountReader::changeAccountFile(QString NewAccountFilename)
{
    if(NewAccountFilename.isEmpty())
        return false;
    currentAccountFile = NewAccountFilename;
    return readAccountFile();
}

bool AccountReader::addAccount(QString displayedName, QString username, QString password, QString lang, QString gfuid, QString cache, int pos, QString filename)
{
    if(gfuid.size() != 36)
        gfuid = "";
    if(cache.size() != 36)
        cache = "";
    if(filename != nullptr)
    {
        if(!accountFileList.contains(filename))
            addAccountFile(filename);
        if(filename != currentAccountFile)
            changeAccountFile(filename);
        currentAccountFile = filename;
    }
    for(size_t i = 0 ; i < currentAccountList.size() ; i++)
    {
        if(displayedName == currentAccountList[i].getDisplayedName())
            return false;
    }
    if(pos == -1)
        currentAccountList.push_back(Account(displayedName, username, password, lang, gfuid, cache));
    else
        currentAccountList.insert(currentAccountList.begin() + pos, Account(displayedName, username, password, lang, gfuid, cache));
    return createAccountFile();
}

bool AccountReader::addAccount(const Account account, int pos)
{
    return addAccount(account.getDisplayedName(), account.getUsername(), account.getPassword(),
                      account.getLang(), account.getGfuid(), account.getCache(), pos, nullptr);
}

bool AccountReader::deleteAccount(size_t position)
{
    if(position >= currentAccountList.size())
        return false;
    currentAccountList.erase(currentAccountList.begin() + static_cast<int>(position));
    return createAccountFile();
}

bool AccountReader::patchAccountCache(QString displayedName, QString cache)
{
    QPair<int, Account> accPair = getAccountData(displayedName);
    Account acc = accPair.second;
    if(acc.getDisplayedName() != displayedName)
        return false;
    if(!deleteAccount(static_cast<size_t>(accPair.first)))
        return false;
    acc.setCache(cache);
    return addAccount(acc, accPair.first);
}

bool AccountReader::addAccountFile(QString filename)
{
    QFile file("accounts/" + filename);
    file.close();
    accountFileList.append(filename);
    return true;
}

QStringList AccountReader::getAccountFileList()
{
    return accountFileList;
}

const std::vector<Account> AccountReader::getAccounts()
{
    return currentAccountList;
}

bool AccountReader::createAccountFile()
{
    QFile file("accounts/" + currentAccountFile);
    if(!file.open(QIODevice::Append))
        return false;
    file.resize(0);
    QTextStream stream(&file);
    for(size_t i = 0 ; i < currentAccountList.size() ; i++)
    {
        const Account a = currentAccountList[i];
        stream << a.getDisplayedName() << "|" << a.getUsername() << "|" << a.getPassword() << "|"
               << a.getLang() << "|" << a.getGfuid() << "|" << a.getCache() << endl;
    }
    file.close();
    return true;
}

bool AccountReader::readAccountFile()
{
    currentAccountList.clear();
    QFile file("accounts/" + currentAccountFile);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream stream(&file);
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList fields = line.split("|");
        if(fields.size() == 6)
            currentAccountList.push_back(Account(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]));
    }
    file.close();
    return true;
}

QPair<size_t, Account> AccountReader::getAccountData(QString displayedName)
{
    for(size_t i = 0 ; i < currentAccountList.size() ; i++)
    {
        if(currentAccountList[i].getDisplayedName() == displayedName)
            return { i, currentAccountList[i] };
    }
    return { 0, Account() };
}





Account::Account()
    : displayedName("")
    , username("")
    , password("")
    , lang("")
    , gfuid("")
    , cache("")
{

}

Account::Account(QString DisplayedName, QString Username, QString Password, QString Lang, QString Gfuid, QString Cache)
    : displayedName(DisplayedName)
    , username(Username)
    , password(Password)
    , lang(Lang)
    , gfuid(Gfuid)
    , cache(Cache)
{}

QString Account::getDisplayedName() const
{
    return displayedName;
}

QString Account::getUsername() const
{
    return username;
}

QString Account::getPassword() const
{
    return password;
}

QString Account::getLang() const
{
    return lang;
}

QString Account::getGfuid() const
{
    return gfuid;
}

QString Account::getCache() const
{
    return cache;
}

void Account::setDisplayedName(QString DisplayedName)
{
    displayedName = DisplayedName;
}

void Account::setUsername(QString Username)
{
    username = Username;
}

void Account::setPassword(QString Password)
{
    password = Password;
}

void Account::setLang(QString Lang)
{
    lang = Lang;
}

void Account::setGfuid(QString Gfuid)
{
    gfuid = Gfuid;
}

void Account::setCache(QString Cache)
{
    cache = Cache;
}
