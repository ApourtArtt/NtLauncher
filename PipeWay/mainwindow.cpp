#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    accountReader = new AccountReader(this);
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/chaussons.png"));
    config = new Configuration();
    initialiseAyugraNamedPipe();
    initialiseGameforgeNamedPipe();
    initialiseTraySystem();
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    settings.setValue("_TNT_CLIENT_APPLICATION_ID", "d3b2a0c1-f0d0-4888-ae0b-1c5e1febdafb");
    settings.setValue("_TNT_SESSION_ID", "12345678-1234-1234-1234-123456789012");
    accAdd = new AccountAdder(accountReader->getAccountFileList(), config->getLang(), this);
    connect(accAdd, &AccountAdder::createAccount, [this](QString displayedName, QString username, QString password, QString lang, QString gfuid, QString cache, QString filename)
    {
        accountReader->addAccount(displayedName, username, password, lang, gfuid, cache, -1, filename);
        ui->CB_ACCOUNTS->clear();
        ui->CB_ACCOUNTS->addItems(accountReader->getAccountFileList());
        reloadAccounts();
    });
    connect(accAdd, &AccountAdder::changedLang, [this](QString Lang){config->setDefaultLang(Lang);});
    reloadAccounts();
    connect(ui->CB_ACCOUNTS, &QComboBox::currentTextChanged, [this](const QString& fn){reloadAccounts(fn);});
    ui->CB_ACCOUNTS->addItems(accountReader->getAccountFileList());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialiseAyugraNamedPipe()
{
    QLocalServer *server = new QLocalServer();
    server->listen("AyugraLauncher");
    connect(server, &QLocalServer::newConnection, [this]
    {
        this->showNormal();
    });
}

void MainWindow::initialiseGameforgeNamedPipe()
{
    QLocalServer *serverGf = new QLocalServer(this);
    serverGf->listen("GameforgeClientJSONRPC");
    connect(serverGf, &QLocalServer::newConnection, this, [=]()
    {
        QLocalSocket *socketGf = serverGf->nextPendingConnection();

        connect(socketGf, &QLocalSocket::readyRead, [=]
        {
            QByteArray msg = socketGf->readAll();
            if(msg.size() > 0)
            {
                socketGf->write(generateResponse(msg));
            }
        });
    });
}

QByteArray MainWindow::generateResponse(QByteArray msg)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
    if (!jsonDoc.isObject()) return{};
    QJsonObject jsonObj = jsonDoc.object();
    QString method = jsonObj.value("method").toString();
    QByteArray response;
    response.append("{\"id\":");
    response.append(QString::number(jsonObj.value("id").toInt()));
    response.append(",\"jsonrpc\":\"2.0\",\"result\":");
    if(method == "ClientLibrary.isClientRunning")
    {
        response.append("true");
    }
    else if(method == "ClientLibrary.initSession")
    {
        response.append("\"" + jsonObj.value("params").toObject().value("sessionId").toString() + "\"");
    }
    else if(method == "ClientLibrary.queryAuthorizationCode")
    {
        response.append("\"" + currentCode + "\"");
    }
    else if(method == "ClientLibrary.queryGameAccountName")
    {
        response.append("\"" + currentUsername + "\"");
    }
    else
        return nullptr;
    response.append("}");
    return response;
}

void MainWindow::on_PB_PARAM_clicked()
{
    config->show();
}

void MainWindow::initialiseTraySystem()
{
    TraySystem *traySystem = new TraySystem(this);
    connect(traySystem, &TraySystem::quit, qApp, &QApplication::quit);
    connect(traySystem, &TraySystem::restore, this, &QWidget::showNormal);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void MainWindow::on_PB_DELETE_clicked()
{
    QList<QListWidgetItem*> itms = ui->LW_ACCOUNTS->selectedItems();
    const std::vector<Account> accs = accountReader->getAccounts();
    if(accs.size() == 0)
        return;
    for(size_t i = accs.size() - 1 ; i < accs.size() ; i--)
    {
        for(int j = 0 ; j < itms.size() ; j++)
        {
            if(itms[j]->text() == accs[i].getDisplayedName())
                accountReader->deleteAccount(i);
        }
    }
    reloadAccounts();
}

void MainWindow::on_PB_CONNECT_clicked()
{
    QList<QListWidgetItem*> itms = ui->LW_ACCOUNTS->selectedItems();
    const std::vector<Account> accs = accountReader->getAccounts();
    QStringList usernames, passwords, langs, gfuids, caches, displayedNames;
    for(int j = 0 ; j < itms.size() ; j++)
    {
        for(size_t i = 0 ; i < accs.size() ; i++)
        {
            if(itms[j]->text() == accs[i].getDisplayedName())
            {
                usernames.push_back(accs[i].getUsername());
                passwords.push_back(accs[i].getPassword());
                langs.push_back(accs[i].getLang());
                gfuids.push_back(accs[i].getGfuid());
                caches.push_back(accs[i].getCache());
                displayedNames.push_back(accs[i].getDisplayedName());
            }
        }
    }
    for(int j = 0 ; j < displayedNames.size() ; j++)
    {
        QTimer::singleShot(config->getAccountTimer() * j, [=]
        {
            CodeGenerator cg(config->getPlatformGameId(), this);
            if(!config->shouldUseCache() || caches[j].size() != 36)
            {
                currentCode = cg.connectToAccount(usernames[j], passwords[j], langs[j], gfuids[j]);
                accountReader->patchAccountCache(displayedNames[j], currentCode);
            }
            else
                currentCode = caches[j];
            if(currentCode.size() != 36)
            {
                ui->statusbar->showMessage(displayedNames[j] + " cannot connect (wrong connection code)", config->getAccountTimer() * (j > 0 ? j : 1));
                return;
            }
            currentUsername = usernames[j];
            QString langAccount = langs[j];
            QProcess proc;
            qint64 pid;
            proc.startDetached(config->getNtDir(), {"gf", QString::number(langToParam.value(langAccount).second)}, QDir::currentPath(), &pid);
            QTimer::singleShot(1250, [=]
            {
                injectDll(QString::number(pid), "Ayugra.dll");
            });
        });
    }
}

void MainWindow::injectDll(QString processId, QString dllPath)
{
    QProcess *proc = new QProcess(this);
    proc->startDetached("\"" + QDir::currentPath() + "/injector.exe\"", { dllPath, processId });
}

void MainWindow::reloadAccounts(QString filename)
{
    accountReader->changeAccountFile(filename);
    ui->LW_ACCOUNTS->clear();
    std::vector<Account> accountList = accountReader->getAccounts();
    for(size_t i = 0 ; i < accountList.size() ; i++)
        ui->LW_ACCOUNTS->insertItem(static_cast<int>(i), accountList[i].getDisplayedName());
}

void MainWindow::on_PB_ACCOUNTMANAGER_clicked()
{
    accAdd->changeLang(config->getLang());
    accAdd->show();
}
