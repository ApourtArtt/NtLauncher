#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QProcess>
#include <QFileDialog>
#include <QtConcurrent>
#include "githubupdate.h"
#include "nostaleupdater.h"

namespace Ui {
class Configuration;
}

class Configuration : public QMainWindow
{
    Q_OBJECT

public:
    explicit Configuration(QMainWindow *parent = nullptr);
    ~Configuration();
    bool readConfiguration();
    bool createConfiguration(QString PlatformGameId, QString NtDir, QString Lang, int AccountTimer, bool KillGf, bool Cache, bool GitUpdate);
    void setDefaultLang(QString Lang);

    QString getPlatformGameId();
    QString getNtDir();
    QString getLang();
    int getAccountTimer();
    bool shouldKillGf();
    bool shouldUseCache();
    bool shouldCheckGitupdate();

signals:
    void NostaleUpdated(bool updated);

private slots:
    void on_PB_BROWSE_clicked();
    void on_CB_CHECK_GITHUB_stateChanged(int arg1);
    void on_CB_CACHE_stateChanged(int arg1);
    void on_CB_KILLGFCLIENT_stateChanged(int arg1);
    void on_SB_TIME_textChanged(const QString &arg1);
    void on_LE_PLATFORMGAMEID_textChanged(const QString &arg1);
    void on_PB_UPDATE_clicked();
    void on_PB_REPAIR_clicked();

private:
    Ui::Configuration *ui;
    NostaleUpdater *ntUpdater;
    QString platformGameId;
    QString ntDir;
    QString lang;
    int accountTimer;
    bool killGf;
    bool cache;
    bool gitUpdate;
};

#endif // CONFIGURATION_H
