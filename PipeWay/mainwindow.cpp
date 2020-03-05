#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialiseAccountList();
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    settings.setValue("_TNT_CLIENT_APPLICATION_ID", "d3b2a0c1-f0d0-4888-ae0b-1c5e1febdafb");
    settings.setValue("_TNT_SESSION_ID", "12345678-1234-1234-1234-123456789012");

    QFile config(CONFIG_FILENAME);
    if(!(config.open(QIODevice::ReadOnly)))
        ui->statusbar->showMessage(tr("config.txt does not exist."));
    else
    {
        QTextStream in(&config);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            if(line.startsWith("NT_DIR="))
                ui->LE_NTDIR->setText(line.remove("NT_DIR="));
            else if(line.startsWith("KILL="))
                ui->CB_KILLGFCLIENT->setChecked(line.remove("KILL=") == "TRUE" ? true : false);
            else if(line.startsWith("TIME="))
                ui->SB_TIME->setValue(line.remove("TIME=").toInt());
            else
                ui->statusbar->showMessage(tr("config.txt is invalid."));
        }
    }
    if(ui->CB_KILLGFCLIENT->isChecked())
        killGfclient();
    time = ui->SB_TIME->value();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PB_CONNECT_clicked()
{
    QList<QListWidgetItem*> items = ui->LW_ACCOUNTS->selectedItems();
    for(int j = 0 ; j < items.size() ;j ++)
    {
        QTimer::singleShot(time * 1000 * j, [=]
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
            {
                connectToAccount(info);
            }
        });
    }
}

void MainWindow::on_PB_ADD_clicked()
{
    QFile file(ACCOUNTS_FILENAME);
    if (!file.open(QIODevice::Append))
    {
        ui->statusbar->showMessage(tr("Something went wrong in accounts.txt opening."));
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

    QProcess *proc = new QProcess(this);
    qint64 pid;
    proc->startDetached(ui->LE_NTDIR->text(), {"gf"}, QDir::currentPath(), &pid);
    QTimer::singleShot(1000, [=]
    {
        injectDll(QString::number(pid), "Ayugra.dll");
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
    QFile file(ACCOUNTS_FILENAME);
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
                ui->statusbar->showMessage(tr("accounts.txt is invalid."));
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
        ui->LE_NTDIR->setText(directory);
    QFile config(CONFIG_FILENAME);
    if(!config.open(QIODevice::ReadWrite))
    {
        ui->statusbar->showMessage(tr("Something went wrong in config.txt opening."));
        return;
    }
    QTextStream stream(&config);
    QStringList line = QString(config.readAll()).split('\n');
    for(int i = 0 ; i < line.size() ; i++)
    {
        if(line[i].startsWith("NT_DIR="))
            line.erase(line.begin() + i);
    }
    config.resize(0);
    line.removeAll("");
    stream << line.join("\n") << endl;
    stream << "NT_DIR=" << directory << endl;
    config.close();
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
    QFile file(ACCOUNTS_FILENAME);
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
        ui->statusbar->showMessage(tr("Something went wrong in accounts.txt opening."));
}

void MainWindow::injectDll(QString processId, QString dllPath)
{
    QProcess *proc = new QProcess(this);
    proc->startDetached("\"" + QDir::currentPath() + "/injector.exe\"", { dllPath, processId });
}

void MainWindow::on_CB_KILLGFCLIENT_stateChanged(int arg1)
{
    QFile config(CONFIG_FILENAME);
    if(!config.open(QIODevice::ReadWrite))
    {
        ui->statusbar->showMessage(tr("Something went wrong in config.txt opening."));
        return;
    }
    QTextStream stream(&config);
    QStringList line = QString(config.readAll()).split('\n');
    for(int i = 0 ; i < line.size() ; i++)
    {
        if(line[i].startsWith("KILL="))
            line.erase(line.begin() + i);
    }
    qDebug() << arg1;
    config.resize(0);
    line.removeAll("");
    stream << line.join("\n") << endl;
    stream << "KILL=" << (arg1 ? "TRUE" : "FALSE") << endl;
    config.close();
}

void MainWindow::on_SB_TIME_valueChanged(const QString &arg1)
{
    QFile config(CONFIG_FILENAME);
    if(!config.open(QIODevice::ReadWrite))
    {
        ui->statusbar->showMessage(tr("Something went wrong in config.txt opening."));
        return;
    }
    QTextStream stream(&config);
    QStringList line = QString(config.readAll()).split('\n');
    for(int i = 0 ; i < line.size() ; i++)
    {
        if(line[i].startsWith("TIME="))
            line.erase(line.begin() + i);
    }
    qDebug() << arg1;
    config.resize(0);
    line.removeAll("");
    stream << line.join("\n") << endl;
    stream << "TIME=" << arg1 << endl;
    config.close();
    time = arg1.toInt();
}

void MainWindow::on_actionGithub_triggered()
{
    QDesktopServices::openUrl(QUrl(tr("https://github.com/ApourtArtt/NtLauncher")));
}
