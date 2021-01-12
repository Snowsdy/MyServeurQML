#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "messagerieserver.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void smbConnected();
    void smbDisconnected();
    void newMessage(QString msg);

public slots:
    void smbConnectedToServer();
    void smbDisconnectedFromServer();
    void gotNewMesssage(QString msg);
private:
    MessagerieServer *server;

};

#endif // BACKEND_H
