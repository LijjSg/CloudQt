#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include "protocol.h"
#include <QString>
#include <QFile>
#include <QTimer>


class myTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    myTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir,QString strDestDir);
public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();

signals:
    void offline(myTcpSocket *mysocket);
private:
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;
    QTimer *m_pTimer;

};

#endif // MYTCPSOCKET_H
