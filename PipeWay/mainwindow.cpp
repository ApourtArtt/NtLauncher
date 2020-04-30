#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if(!QDir("accounts").exists())
        QDir().mkdir("accounts");
    QDir dir = QDir::currentPath() + "/accounts";
    QStringList accounts = dir.entryList({"*.txt"});
    if(accounts.size() > 0)
        currentAccountFile = accounts[0];
    else
    {
        currentAccountFile = "accounts1.txt";
        accounts.append("accounts1.txt");
    }
    ui->setupUi(this);

    NetworkRequester netReq;
    QNetworkRequest req(QUrl("https://api.github.com/repos/ApourtArtt/NtLauncher/releases/latest"));
    QByteArray response = netReq.get(req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    if(jsonObj.value(QString("tag_name")).toString() > VERSION)
    {
        QAction *update = new QAction("An update is available", this);
        ui->menuBar->addAction(update);
        connect(update, &QAction::triggered, [=]
        {
            UpdateWidget *upWidg = new UpdateWidget(jsonObj);
            upWidg->show();
        });
    }

    for(int i = 0 ; i < accounts.size() ; i++)
        ui->CB_ACCOUNTS->insertItem(i, accounts[i]);
    time = 12000;
    ntdir = "";
    kill = false;
    lang = "EN";
    if(!readConfigFile())
        ui->statusbar->showMessage("Config file is invalid. Options are automatically saved.");
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    settings.setValue("_TNT_CLIENT_APPLICATION_ID", "d3b2a0c1-f0d0-4888-ae0b-1c5e1febdafb");
    settings.setValue("_TNT_SESSION_ID", "12345678-1234-1234-1234-123456789012");
    initialiseAccountList();
    if(kill) killGfclient();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PB_CONNECT_clicked()
{
    QList<QListWidgetItem*> items = ui->LW_ACCOUNTS->selectedItems();
    for(int j = 0 ; j < items.size() ; j++)
    {
        QTimer::singleShot(time * j, [=]
        {
            QStringList accountInfo = getAccountInfos();
            QStringList info;
            for(int i = 0 ; i < accountInfo.size() ; i = i + 4)
            {
                if(accountInfo[i] == items[j]->text())
                {
                    for(int w = 0 ; w < 4 ; w++)
                        info.push_back(accountInfo[i + w]);
                }
            }
            if(!info.isEmpty())
                connectToAccount(info);
        });
    }
}

void MainWindow::on_PB_ADD_clicked()
{
    QFile file("accounts/" + currentAccountFile);
    if (!file.open(QIODevice::Append))
    {
        ui->statusbar->showMessage(tr("Something went wrong in " + currentAccountFile.toUtf8() + " opening."));
        return;
    }
    QTextStream stream(&file);
    stream << ui->LE_USERNAME->text() << "|" << ui->LE_PASSWORD->text() << "|" << ui->LE_GFUID->text() << "|" << ui->CB_LANG->currentText() << endl;
    file.close();
    ui->LW_ACCOUNTS->addItem(ui->LE_USERNAME->text());
}

void MainWindow::connectToAccount(QStringList infos)
{
    CodeGenerator cg;
    code = cg.connectToAccount(infos[0], infos[1], infos[3], infos[2]);
    gfuid = cg.getGfuid();
    username = infos[0];
    QString langAccount = infos[3];

    QProcess *proc = new QProcess(this);
    qint64 pid;
    proc->startDetached(ntdir, {"gf", QString::number(langToId.value(langAccount))}, QDir::currentPath(), &pid);
    QTimer::singleShot(1000, [=]
    {
        injectDll(QString::number(pid), "tmp/" + QString::number(pid) + ".dll");
    });
    QLocalServer *server = new QLocalServer(this);
    server->listen("GameforgeClientJSONRPC");
    connect(server, &QLocalServer::newConnection, this, [=]()
    {
        QLocalSocket *socket = server->nextPendingConnection();
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]()
        {
            QByteArray msg = socket->readAll();
            if(msg.size() > 0)
            {
                socket->write(generateResponse(msg));
                timer->stop();
            }
        });
        timer->start(5);
    });
}

QStringList MainWindow::getAccountInfos()
{
    QStringList infos;
    QFile file("accounts/" + currentAccountFile);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split("|");
            if(fields.size() == 4)
                infos << fields;
            else
                ui->statusbar->showMessage(tr(currentAccountFile.toUtf8() + " is invalid."));
        }
    }
    else
        ui->statusbar->showMessage(tr("No account loaded"), 5000);
    file.close();
    return infos;
}

void MainWindow::initialiseAccountList()
{
    ui->LW_ACCOUNTS->clear();
    QStringList infos = getAccountInfos();
    for (int i = 0 ; i < infos.size() ; i  = i + 4)
        ui->LW_ACCOUNTS->addItem(infos[i]);
}

void MainWindow::killGfclient()
{
    QProcess::execute("taskkill /im gfclient.exe /f");
}

void MainWindow::on_PB_BROWSE_clicked()
{
    QString directory = QFileDialog::getOpenFileName(this, tr("Select NostaleClientX.exe"), QDir::currentPath(), "NostaleClientX.exe");
    if (!directory.isEmpty())
    {
        ui->LE_NTDIR->setText(directory);
        ntdir = directory;
        createConfigFile();
    }
    ui->LE_NTDIR->setText(directory);
}

QByteArray MainWindow::generateResponse(QByteArray msg)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
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
        response.append("\"" + jsonObj.value("sessionId").toString() + "\"");
    }
    else if(method == "ClientLibrary.queryAuthorizationCode")
    {
        response.append("\"" + code + "\"");
    }
    else if(method == "ClientLibrary.queryGameAccountName")
    {
        response.append("\"" + username + "\"");
    }
    else
        return nullptr;
    response.append("}");
    return response;
}

void MainWindow::on_PB_DELETE_clicked()
{
    QFile file("accounts/" + currentAccountFile);
    if(file.open(QIODevice::ReadWrite))
    {
        QString s;
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split("|");
            if(!(fields[0] == ui->LW_ACCOUNTS->currentItem()->text()))
                s.append(line + "\n");
        }
        file.resize(0);
        in << s;
        file.close();
        ui->LW_ACCOUNTS->takeItem(ui->LW_ACCOUNTS->currentRow());
    }
    else
        ui->statusbar->showMessage(tr("Something went wrong in " + currentAccountFile.toUtf8() + " opening."));
}

void MainWindow::injectDll(QString processId, QString dllPath)
{
    QProcess *proc = new QProcess(this);
    proc->startDetached("\"" + QDir::currentPath() + "/injector.exe\"", { dllPath, processId });
}

void MainWindow::on_actionGithub_triggered()
{
    QDesktopServices::openUrl(QUrl(tr("https://github.com/ApourtArtt/NtLauncher")));
}

void MainWindow::on_CB_ACCOUNTS_currentIndexChanged(const QString &arg1)
{
    currentAccountFile = arg1;
    initialiseAccountList();
}

/* config.json
 * {
 *     "ntdir": "C:\\...",
 *     "kill": "true",
 *     "time": "...",
 *     "language" : "fR"
 * }
*/

bool MainWindow::createConfigFile()
{
    QJsonObject json;

    json["ntdir"]   = ntdir == ""   ? ui->LE_NTDIR->text()              : ntdir;
    json["lang"]    = lang == ""    ? ui->CB_LANG->currentText()        : lang;
    json["time"]    = time < 0      ? ui->SB_TIME->value()              : time;
    json["kill"]    = kill == true  ? ui->CB_KILLGFCLIENT->isChecked()  : kill;

    QFile saveFile(QStringLiteral("config.json"));
    if (!saveFile.open(QIODevice::WriteOnly))
        return false;
    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    return true;
}

bool MainWindow::readConfigFile()
{
    QJsonObject json;
    QFile loadFile(QStringLiteral("config.json"));
    if (!loadFile.open(QIODevice::ReadOnly))
        return false;
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    json = loadDoc.object();

    if(json.contains("ntdir") && json["ntdir"].isString())
    {
        ntdir = loadDoc["ntdir"].toString();
        ui->LE_NTDIR->setText(ntdir);
    }
    else return false;

    if(json.contains("lang") && json["lang"].isString())
    {
        lang = loadDoc["lang"].toString();
        ui->CB_LANG->setCurrentText(lang);
    }
    else return false;

    if(json.contains("time") && json["time"].isDouble())
    {
        time = loadDoc["time"].toDouble();
        ui->SB_TIME->setValue(time / 1000);
    }
    else return false;

    if(json.contains("kill") && json["kill"].isBool())
    {
        kill = loadDoc["kill"].toBool();
        ui->CB_KILLGFCLIENT->setChecked(kill);
    }
    else return false;

    return true;
}

void MainWindow::on_CB_LANG_currentIndexChanged(const QString &arg1)
{
    lang = arg1;
    createConfigFile();
}

void MainWindow::on_SB_TIME_valueChanged(double arg1)
{
    time = static_cast<int>(arg1 * 1000);
    createConfigFile();
}

void MainWindow::on_CB_KILLGFCLIENT_stateChanged(int arg1)
{
    kill = arg1;
    createConfigFile();
}
