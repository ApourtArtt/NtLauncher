#include "pipemanager.h"

PipeManager::PipeManager(QObject *parent)
    : QObject(parent)
{
    server = new QLocalServer(this);
    server->setMaxPendingConnections(255);
    server->listen("Ayugra");
    connect(server, &QLocalServer::newConnection, this, [=]()
    {
        QPair<size_t, QLocalSocket*> socket;
        socket.first = sockets.size();
        socket.second = server->nextPendingConnection();
        socket.second->write(connectionPacket);
        sockets.push_back(socket);
        connect(socket.second, &QLocalSocket::disconnected, this, [=]()
        {

        });
        connect(socket.second, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), [=](QLocalSocket::LocalSocketError socketError)
        {

        });
    });
}

bool PipeManager::connectAccount(QString Code, QString Un, QString Gfid)
{
    if(code.isEmpty() || Un.isEmpty() || Gfid.isEmpty())
        return false;
    code = Code;
    username = Un;
    gfuid = Gfid;
    connectionPacket.clear();
    connectionPacket.append(code);
    connectionPacket.append("|");
    connectionPacket.append(username);
    connectionPacket.append("|");
    connectionPacket.append(gfuid);
    return true;
}
