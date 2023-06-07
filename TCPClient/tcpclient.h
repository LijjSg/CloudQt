#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();
    void loadConfig();
    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();

    QString loginName();
    QString curPath();
    void setCurPath(QString strCurPath);


public slots:
    void showConnetc();
    void recvMsg();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    // 连接服务器，和服务器数据进行交互
    QTcpSocket m_tcpSocket;
    QString m_strName;
    QString m_strCurPath;
    QFile m_file;


};

#endif // TCPCLIENT_H
