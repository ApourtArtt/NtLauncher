#include "connectionwindow.h"
#include <QDebug>

ConnectionWindow::ConnectionWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);
    initialiseAccountList();
    QFile config(CONFIGURATION_FILENAME);
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
            else
                ui->statusbar->showMessage(tr("config.txt is invalid."));
        }
    }
    engine = new Engine(ui->LE_NTDIR->text(), this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

QStringList ConnectionWindow::getAccountInfos()
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
            if(fields.size() == 3)
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

void ConnectionWindow::on_PB_CONNECT_clicked()
{
    if(ui->LW_ACCOUNTS->currentItem())
    {
        QStringList accountInfo = getAccountInfos();
        QStringList info;
        for(int i = 0 ; i < accountInfo.size() ; i = i + 3)
        {
            if(accountInfo[i] == ui->LW_ACCOUNTS->currentItem()->text())
            {
                for(int j = 0 ; j < 3 ; j++)
                    info.push_back(accountInfo[i + j]);
            }
        }
        if(!info.isEmpty())
            engine->connectToAccount(info[0], info[1], info[2]);
    }
}

void ConnectionWindow::on_PB_ADD_clicked()
{
    QFile file(ACCOUNTS_FILENAME);
    if (!file.open(QIODevice::Append))
    {
        ui->statusbar->showMessage(tr("Something went wrong in accounts.txt opening."));
        return;
    }
    QTextStream stream(&file);
    stream << ui->LE_USERNAME->text() << "|" << ui->LE_PASSWORD->text() << "|" << ui->LE_GFUID->text() << endl;
    file.close();
    ui->LW_ACCOUNTS->addItem(ui->LE_USERNAME->text());
}

void ConnectionWindow::initialiseAccountList()
{
    ui->LW_ACCOUNTS->clear();
    QStringList infos = getAccountInfos();
    for (int i = 0 ; i < infos.size() ; i  = i + 3)
        ui->LW_ACCOUNTS->addItem(infos[i]);
}

void ConnectionWindow::on_PB_BROWSENTDIR_clicked()
{
    QString directory = QFileDialog::getOpenFileName(this, tr("Select NostaleClientX.exe"), QDir::currentPath(), "NostaleClientX.exe");
    if (!directory.isEmpty())
        ui->LE_NTDIR->setText(directory);
    QFile config(CONFIGURATION_FILENAME);
    if(!config.open(QIODevice::Append))
    {
        ui->statusbar->showMessage(tr("Something went wrong in config.txt opening."));
        return;
    }
    QTextStream stream(&config);
    stream << "NT_DIR=" << directory << endl;
    config.close();
    ui->LE_NTDIR->setText(directory);
    engine->changeNostaleFile(directory);
}
