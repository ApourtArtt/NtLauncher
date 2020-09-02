#ifndef TRAYSYSTEM_H
#define TRAYSYSTEM_H

#include <QSystemTrayIcon>
#include <QMenu>

class TraySystem : public QObject
{
    Q_OBJECT
public:
    TraySystem(QWidget *parent);

signals:
    void quit();
    void restore();
};

#endif // TRAYSYSTEM_H
