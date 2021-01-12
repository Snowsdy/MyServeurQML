#include "messagerieserver.h"
#include <QDateTime>
#include <unistd.h>
#include <QOperatingSystemVersion>

MessagerieServer::MessagerieServer(): QObject(), m_nNextBlockSize(0)
{
    tcpServer = new QTcpServer();
    survey = false;
}

QList<QTcpSocket *> MessagerieServer::getClients()
{
    return mSockets;
}

void MessagerieServer::newConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &MessagerieServer::readClient);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MessagerieServer::gotDisconnection);

    mSockets << clientSocket;

    sendToClient(clientSocket, "Connexion établie");
}

void MessagerieServer::readClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream entre(clientSocket);
    int verif = 0;

    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) { break; }
            entre >> m_nNextBlockSize;
        }

        if (clientSocket->bytesAvailable() < m_nNextBlockSize) { break; }
        QString str;
        entre >> str;

        if (str.contains("count()")){
            str = "Nombre de clients connectés : " + QString::number(this->mSockets.count());
        }
        if (str.contains("getpid()")){
            str = "Pid du serveur : " + QString::number(getpid());
        }
        if (str.contains("survey()")){
            if (!survey)
                str = "Serveur : Demande de surveillance activée.";
            else
                str = "Serveur : Demande de surveillance désactivée.";
            survey = !survey;
        }
        if (str.contains("kill()")){
            bool onWindows = ( QOperatingSystemVersion::Windows == QOperatingSystemVersion::currentType() );
            int pos = str.indexOf(')');
            str.remove(0, pos + 2);
            if (onWindows){
                str = "taskkill /F /PID " + str;
                system(str.toLatin1().data());
            }else{
                str = "kill -9 " + str;
                system(str.toLatin1().data());
            }
        }
        if (str.contains("help()")){
            str = "Commandes disponibles : help(), getpid(), count(), survey(), kill(), clear().";
        }

        emit gotNewMesssage(str);

        m_nNextBlockSize = 0;

        for (auto socket : mSockets){
            verif = sendToClient(socket, QString(str));
        }

        if (survey)
        {
            qDebug() << "[" << QDateTime::currentDateTime().toString() << "]" << ":" << str;
        }

        if (verif == -1)
            qDebug() << "Une erreur est survenu lors de l'envoi du message";
    }
}

void MessagerieServer::gotDisconnection()
{
    mSockets.removeAt(mSockets.indexOf((QTcpSocket*)sender()));
    qDebug() << "Un client s'est deconnecte";
    for (auto socket : mSockets){
        sendToClient(socket, "Un client s'est déconnecté");
    }
    emit smbDisconnected();
}

qint64 MessagerieServer::sendToClient(QTcpSocket *socket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    return socket->write(arrBlock);
}
