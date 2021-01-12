#ifndef MESSAGERIESERVER_H
#define MESSAGERIESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QList>

class MessagerieTcp;

class MessagerieServer : public QObject
{
    Q_OBJECT
public:
    MessagerieServer();
    QTcpServer *tcpServer;
    bool startServer(quint16 port);
    QList<QTcpSocket *> getClients();
public slots:
    virtual void newConnection();
    void readClient();
    void gotDisconnection();
    qint64 sendToClient(QTcpSocket *socket, const QString &str);
signals:
    void gotNewMesssage(QString msg);
    void smbDisconnected();
private:
    QList<QTcpSocket *> mSockets;
    quint16 m_nNextBlockSize;
    bool survey;
};

#endif // MESSAGERIESERVER_H
