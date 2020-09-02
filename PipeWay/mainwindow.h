#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "codegenerator.h"
#include "configuration.h"
#include "traysystem.h"
#include "accountreader.h"
#include "accountadder.h"

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
    void on_PB_PARAM_clicked();
    void on_PB_DELETE_clicked();
    void on_PB_CONNECT_clicked();
    void on_PB_ACCOUNTMANAGER_clicked();

private:
    void closeEvent(QCloseEvent *event);
    void initialiseTraySystem();
    void initialiseAyugraNamedPipe();
    void initialiseGameforgeNamedPipe();

    QByteArray generateResponse(QByteArray msg);
    void injectDll(QString processId, QString dllPath);

    void reloadAccounts(QString filename = nullptr);

    QString currentCode, currentUsername;

    Ui::MainWindow *ui;
    Configuration *config;
    AccountReader *accountReader;
    AccountAdder *accAdd;
};
#endif // MAINWINDOW_H
