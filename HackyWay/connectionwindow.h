#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "engine.h"
#include "ui_connectionwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ConnectionWindow; }
QT_END_NAMESPACE

class ConnectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConnectionWindow(QWidget *parent = nullptr);
    ~ConnectionWindow();

private slots:
    void on_PB_CONNECT_clicked();
    void on_PB_ADD_clicked();

    void on_PB_BROWSENTDIR_clicked();

private:
    void initialiseAccountList();
    QStringList getAccountInfos();
    void createNostalePatched();

    Ui::ConnectionWindow *ui;
    Engine *engine;

    QString ACCOUNTS_FILENAME = "accounts.txt";
    QString CONFIGURATION_FILENAME = "config.txt";
};

#endif // CONNECTIONWINDOW_H
