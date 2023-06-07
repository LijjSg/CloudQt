#include "mytcpserver.h"
#include "mytcpsocket.h"
#include <QTextCodec>
#include <QTextStream>

MyTcpServer::MyTcpServer()
{


}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;

}

// 当一个新的TCP客户端连接到服务器时，Qt会自动调用该函数。
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connect!";
    myTcpSocket *pTcpSocket = new myTcpSocket;
    pTcpSocket->setSocketOption(QTcpSocket::KeepAliveOption, true);

    QTextStream stream(pTcpSocket);
    stream.setCodec("UTF-8");
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(offline(myTcpSocket*)),this,SLOT(deleteSocket(myTcpSocket*)));
}

void MyTcpServer::resend(const char *name, PDU *pdu)
{
    if(name == NULL || pdu == NULL) return;
    QString strname = name;

    for(int i = 0; i < m_tcpSocketList.size(); i++){
        if(strname == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            qDebug() << strname;
            break;
    }
}
}

void MyTcpServer::deleteSocket(myTcpSocket *mysocket)
{
    QList<myTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter++){
        if(mysocket == *iter){
            (*iter)->deleteLater();
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }

    for(int i = 0; i < m_tcpSocketList.size(); i++){
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}
