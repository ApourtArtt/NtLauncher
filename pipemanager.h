#ifndef PIPEMANAGER_H
#define PIPEMANAGER_H

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>

class PipeManager : public QObject
{
    Q_OBJECT
public:
    explicit PipeManager(QObject *parent = nullptr);
    bool connectAccount(QString Code, QString Un, QString Gfid);

private:
    QLocalServer *server;
    std::vector<QPair<size_t, QLocalSocket*>> sockets;
    QString code, username, gfuid;
    QByteArray connectionPacket;
};

#endif // PIPEMANAGER_H
