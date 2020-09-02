#include "configuration.h"
#include "ui_configuration.h"

/* config.json
 * {
 *     "platformGameId": "QString"
 *     "ntdir": "QString",
 *     "kill": "bool",
 *     "time": "int (ms)",
 *     "language" : "QString",
 *     "cache" : "bool",
 *     "gitupdate" : "bool"
 * }
*/

Configuration::Configuration(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/chaussons.png"));
    if(!readConfiguration())
        createConfiguration("dd4e22d6-00d1-44b9-8126-d8b40e0cd7c9", "C:\\Program Files (x86)\\Nostale\\fr-FR\\NostaleClientX.exe", "FR", 15000, true, true, true);
    if(killGf)
        QProcess::execute("taskkill /im gfclient.exe /f");
    if(gitUpdate)
    {
        GithubUpdate *githubUpdate = new GithubUpdate(parent);
        Q_UNUSED(githubUpdate)
    }
    ui->LE_PLATFORMGAMEID->setText(platformGameId);
    ui->SB_TIME->setValue(accountTimer / 1000);
    ui->CB_CACHE->setChecked(cache);
    ui->CB_CHECK_GITHUB->setChecked(gitUpdate);
    ui->CB_KILLGFCLIENT->setChecked(killGf);
    ui->LE_NTDIR->setText(ntDir);
    ntUpdater = new NostaleUpdater(this);
    QDir dir(ntDir);
    dir.cdUp();
    ntUpdater->changeFolder(dir);
    connect(ntUpdater, &NostaleUpdater::resultUpdated, [=](bool result)
    {
        if(result)
            ui->statusBar->showMessage("NosTale is updated");
        else
            ui->statusBar->showMessage("An update is available");
        ui->PB_REPAIR->setEnabled(true);
        ui->PB_UPDATE->setEnabled(true);
    });
}

Configuration::~Configuration()
{
    delete ui;
}

bool Configuration::readConfiguration()
{
    QJsonObject json;
    QFile loadFile(QStringLiteral("config.json"));
    if (!loadFile.open(QIODevice::ReadOnly))
        return false;
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    json = loadDoc.object();

    if(json.contains("platformGameId") && json["platformGameId"].isString())
        platformGameId = loadDoc["platformGameId"].toString().toLower();
    else return false;

    if(json.contains("ntdir") && json["ntdir"].isString())
        ntDir = loadDoc["ntdir"].toString();
    else return false;

    if(json.contains("lang") && json["lang"].isString())
        lang = loadDoc["lang"].toString().toUpper();
    else return false;

    if(json.contains("time"))
        accountTimer = loadDoc["time"].toInt();
    else return false;

    if(json.contains("kill") && json["kill"].isBool())
        killGf = loadDoc["kill"].toBool();
    else return false;

    if(json.contains("cache") && json["cache"].isBool())
        cache = loadDoc["cache"].toBool();
    else return false;

    if(json.contains("gitupdate") && json["gitupdate"].isBool())
        gitUpdate = loadDoc["gitupdate"].toBool();
    else return false;

    return true;
}

bool Configuration::createConfiguration(QString PlatformGameId, QString NtDir, QString Lang, int AccountTimer, bool KillGf, bool Cache, bool GitUpdate)
{
    QJsonObject json;

    json["platformGameId"]  = PlatformGameId.toLower();
    json["ntdir"]           = NtDir;
    json["lang"]            = Lang.toUpper();
    json["time"]            = AccountTimer;
    json["kill"]            = KillGf;
    json["cache"]           = Cache;
    json["gitupdate"]       = GitUpdate;

    platformGameId = PlatformGameId;
    ntDir = NtDir;
    lang = Lang;
    accountTimer = AccountTimer;
    killGf = KillGf;
    cache = Cache;
    gitUpdate = GitUpdate;

    QFile saveFile(QStringLiteral("config.json"));
    if (!saveFile.open(QIODevice::WriteOnly))
        return false;
    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    return true;
}

void Configuration::setDefaultLang(QString Lang)
{
    lang = Lang;
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

QString Configuration::getPlatformGameId()
{
    return platformGameId;
}

QString Configuration::getNtDir()
{
    return ntDir;
}

QString Configuration::getLang()
{
    return lang;
}

int Configuration::getAccountTimer()
{
    return accountTimer;
}

bool Configuration::shouldKillGf()
{
    return killGf;
}

bool Configuration::shouldUseCache()
{
    return cache;
}

bool Configuration::shouldCheckGitupdate()
{
    return gitUpdate;
}

void Configuration::on_PB_BROWSE_clicked()
{
    QString directory = QFileDialog::getOpenFileName(this, tr("Select NostaleClientX.exe"), QDir::currentPath(), "NostaleClientX.exe");
    if (!directory.isEmpty())
    {
        ui->LE_NTDIR->setText(directory);
        ntDir = directory;
        createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
    }
    ui->LE_NTDIR->setText(directory);
}

void Configuration::on_CB_CHECK_GITHUB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    gitUpdate = ui->CB_CHECK_GITHUB->isChecked();
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

void Configuration::on_CB_CACHE_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    cache = ui->CB_CACHE->isChecked();
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

void Configuration::on_CB_KILLGFCLIENT_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    killGf = ui->CB_KILLGFCLIENT->isChecked();
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

void Configuration::on_SB_TIME_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    accountTimer = static_cast<int>(ui->SB_TIME->value() * 1000);
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

void Configuration::on_LE_PLATFORMGAMEID_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    platformGameId = ui->LE_PLATFORMGAMEID->text();
    createConfiguration(platformGameId, ntDir, lang, accountTimer, killGf, cache, gitUpdate);
}

void Configuration::on_PB_UPDATE_clicked()
{
    QDir dir(ntDir);
    dir.cdUp();
    ntUpdater->changeFolder(dir);
    ui->PB_REPAIR->setEnabled(false);
    ui->PB_UPDATE->setEnabled(false);
    if(ntUpdater->isUpdated())
        ui->statusBar->showMessage("NosTale is already up-to-date");
    else
    {
        ui->statusBar->showMessage("Verifying remote files...");
        QtConcurrent::run([=]
        {
            if(!ntUpdater->checkForUpdate())
                ntUpdater->update();
        });
    }
}

void Configuration::on_PB_REPAIR_clicked()
{
    QDir dir(ntDir);
    dir.cdUp();
    ntUpdater->changeFolder(dir);
    ui->PB_REPAIR->setEnabled(false);
    ui->PB_UPDATE->setEnabled(false);
    ui->statusBar->showMessage("Getting new files...");
    QtConcurrent::run([=]
    {
        ntUpdater->isUpdated();
        ntUpdater->repair();
    });
}
