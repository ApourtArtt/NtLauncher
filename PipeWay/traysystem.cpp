#include "traysystem.h"

TraySystem::TraySystem(QWidget *parent)
    : QObject(parent)
{
    QMenu *trayIconMenu = new QMenu(parent);
    QAction *restoreAction  = new QAction(tr("&Restore"), this);
    QAction *quitAction     = new QAction(tr("&Quit"), this);
    connect(restoreAction, &QAction::triggered, [this]{emit restore();} ); // &QWidget::showNormal
    connect(quitAction, &QAction::triggered, [this]{emit quit();});//qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason)
    {
        if(reason == QSystemTrayIcon::ActivationReason::DoubleClick)
            emit restore();
    });
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/chaussons.png"));
    trayIcon->setToolTip("Ayugra");
    trayIcon->show();
}
