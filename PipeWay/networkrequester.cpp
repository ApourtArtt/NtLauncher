#include "networkrequester.h"

NetworkRequester::NetworkRequester(QObject *parent)
    : QObject(parent)
{

}

QByteArray NetworkRequester::post(QByteArray jsonRequest, QNetworkRequest header)
{
    QSharedPointer<QNetworkReply> reply = QSharedPointer<QNetworkReply>(nam.post(header, jsonRequest));
    while (!reply->isFinished())
        qApp->processEvents();
    return reply->readAll();
}

QByteArray NetworkRequester::get(QNetworkRequest header)
{
    QSharedPointer<QNetworkReply> reply = QSharedPointer<QNetworkReply>(nam.get(header));
    while(!reply->isFinished())
        qApp->processEvents();
    return reply->readAll();
}
