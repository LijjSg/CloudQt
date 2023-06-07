#include "tcpsever.h"
#include "ui_tcpsever.h"
#include "mytcpserver.h"
#include <QIODevice>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

TcpSever::TcpSever(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpSever)
{

    loadConfig();//加载配置文件
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
    ui->setupUi(this);
}

// 配置文件加载
void TcpSever::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
   //     qDebug() << strData ;
        file.close();
        // 字符串分割
        strData.replace("\r\n"," ");
        qDebug() << strData;
        QStringList strList = strData.split(" ");
//        for(int i = 0; i < strList.size(); i++){
//            qDebug() << "--->" << strList[i];
//        }
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        qDebug() << "IP: " << m_strIP;
        qDebug() << "Port:" << m_usPort;

    }
    else
    {
        QMessageBox::critical(this,"open config" ,"open faile");
    }
}



TcpSever::~TcpSever()
{
    delete ui;
}
