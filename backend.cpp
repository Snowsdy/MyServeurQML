#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent)
{
    server = new MessagerieServer();
    connect(server, &MessagerieServer::gotNewMesssage, this, &Backend::gotNewMesssage);
    connect(server->tcpServer, &QTcpServer::newConnection, this, &Backend::smbConnectedToServer);
    connect(server, &MessagerieServer::smbDisconnected, this, &Backend::smbDisconnectedFromServer);

    if (!server->tcpServer->listen(QHostAddress::Any, 3333))
    {
        qDebug() << "Erreur ! Le port est deja utilise par un autre service";
    }
    else
    {
        connect(server->tcpServer, &QTcpServer::newConnection, server, &MessagerieServer::newConnection);
        qDebug() << "Server lance, le port est ouvert";
    }
}

void Backend::smbConnectedToServer()
{
    emit smbConnected();
}

void Backend::smbDisconnectedFromServer()
{
    emit smbDisconnected();
}

void Backend::gotNewMesssage(QString msg)
{
    emit newMessage(msg);
}
