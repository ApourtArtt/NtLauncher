#ifndef NETWORKREQUESTER_H
#define NETWORKREQUESTER_H

#include <QObject>
#include <QtNetwork>

class NetworkRequester : public QObject
{
    Q_OBJECT
public:
    explicit NetworkRequester(QObject *parent = nullptr);
    QByteArray post(QByteArray jsonRequest, QNetworkRequest header);
    QByteArray get(QNetworkRequest header);

private:
    QNetworkAccessManager nam;
};

#endif // NETWORKREQUESTER_H
