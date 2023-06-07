#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QTcpServer>
#include "mytcpsocket.h"
#include <QList>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescriptor);
    void resend(const char *name,PDU* pdu);
private:
    QList<myTcpSocket*> m_tcpSocketList;
public slots:
    void deleteSocket(myTcpSocket *mysocket);

};

#endif // MYTCPSERVER_H
