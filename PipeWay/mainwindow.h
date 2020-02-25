#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFileDialog>
#include <QSettings>
#include "ui_mainwindow.h"
#include "codegenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_PB_CONNECT_clicked();
    void on_PB_ADD_clicked();
    void on_PB_BROWSE_clicked();
    void on_PB_DELETE_clicked();

private:
    void injectDll(QString processId, QString dllPath);
    QByteArray generateResponse(QByteArray msg);
    void connectToAccount(QStringList infos);
    QStringList getAccountInfos();
    void initialiseAccountList();
    Ui::MainWindow *ui;

    QString code, username, gfuid;

   const QString ACCOUNTS_FILENAME  = "accounts.txt";
   const QString CONFIG_FILENAME    = "config.txt";
};
#endif // MAINWINDOW_H
