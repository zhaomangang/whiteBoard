#ifndef CLIENT_H
#define CLIENT_H
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>


class Client
{
public:
    Client(int port,QHostAddress *serverIP,QString userName);

private:
    bool status;
    int port;
    QHostAddress *serverIP;
    QString userName;
    QTcpSocket *tcpSocket;

signals:
    void updateServer(QString,int);
};

#endif // CLIENT_H
