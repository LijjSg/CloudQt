#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <iostream>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"
#include "opeweight.h"
#include "privatechat.h"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
     //第一个参数是信号的发送方，第二个参数未发送的是什么信号，第三个参数是信号的接收方，第四个参数是用什么函数进行处理；只要发送那个信号之后，就会调用函数去处理
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnetc()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{

    delete ui;
}

// 配置文件加载
void TcpClient::loadConfig()
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{

    return m_strName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath = strCurPath;
}

void TcpClient::showConnetc()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    if(!OpeWeight::getInstance().getBook()->getDownLoad()){
    qDebug() << m_tcpSocket.bytesAvailable(); // 当前可读的数据
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint),uiPDULen - sizeof(uint));

    //qDebug() << pdu->uiMesgType << ":" << (char*)pdu->caMsg;
    switch (pdu->uiMesgType) {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if(0 == strcmp(pdu->caData,REGIST_OK)){
            QMessageBox::information(this,"注册","注册成功！");
        }else if(0 == strcmp(pdu->caData,REGIST_FAILED)){
            QMessageBox::information(this,"注册","注册失败！");
        }
        break;
    }
    case ENUM_MSG_LOGIN_RESPOND:
        {
            if(0 == strcmp(pdu->caData,LOGIN_OK)){
                m_strCurPath = QString("./%1").arg(m_strName);
                QMessageBox::information(this,"登录","登录成功！");
                OpeWeight::getInstance().show();
                this->hide();
            }else if(0 == strcmp(pdu->caData,LOGIN_FAILED)){
                QMessageBox::information(this,"登录","登录失败！");
            }
            break;
        }
    case ENUM_MSG_ALL_ONLINE_RESPOND:
    {
        OpeWeight::getInstance().getFriend()->showAllOnlie(pdu);
        break;
    }
    case ENUM_MSG_SEARCH_RESPOND:
    {
       if(strcmp(SEARCG_USR_NO,pdu->caData) == 0){
           QMessageBox::information(this,"搜索",QString("%1: not Find").arg(OpeWeight::getInstance().getFriend()->m_strSearchName));
       }else if(strcmp(SEARCH_USR_ONLINE,pdu->caData) == 0)
       {
           QMessageBox::information(this,"搜索",QString("%1: online").arg(OpeWeight::getInstance().getFriend()->m_strSearchName));
       }else if(strcmp(SEARCH_USR_OFFLINE,pdu->caData) == 0)
       {
           QMessageBox::information(this,"搜索",QString("%1: offline").arg(OpeWeight::getInstance().getFriend()->m_strSearchName));
       }
       break;
    }
    case ENUM_MSG_ADD_FRIEND_REQUEST:
    {
        char caName[32] = {'\0'};
        strncpy(caName,pdu->caData+32,32);
        int ret = QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend?").arg(caName),QMessageBox::Yes,QMessageBox::No);
        PDU *respdu = mkPDU(0);
        memcpy(respdu->caData,pdu->caData,64);
        qDebug() << respdu->caData;
        if(ret == QMessageBox::Yes){
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_AGGREE;
        }else{
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"添加好友"
                                 ,pdu->caData);
        break;
    }
    case ENUM_MSG_FLUSH_FRIEND_RESPOND:
    {
        OpeWeight::getInstance().getInstance().getFriend()->updataFrind(pdu);
        break;
    }
    case ENUM_MSG_DEL_FRIEND_REQUEST:
    {
        char caName[32] = {'\0'};
        memcpy(caName,pdu->caData,32);
        QMessageBox::information(this,"删除好友",QString("%1 删除你的好友").arg(caName));
        break;
    }
    case ENUM_MSG_DEL_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    case ENUM_MSG_PRIVATE_CHAT_REQUEST:
    {

        if(PrivateChat::getInstance().isHidden()){

            PrivateChat::getInstance().show();
        }
        char caSendName[32] = {'\0'};
        memcpy(caSendName,pdu->caData,32);
        QString strSendName = caSendName;
        PrivateChat::getInstance().setChatName(strSendName);
        PrivateChat::getInstance().updateMsg(pdu);
        break;
    }
    case ENUM_MSG_GROUP_CHAT_REQUEST:
    {
        OpeWeight::getInstance().getInstance().getFriend()->updateGroupMsg(pdu);
        break;
    }
    case ENUM_MSG_CREATE_DIR_RESPOND:
    {
        QMessageBox::information(this,"创建文件",pdu->caData);
        break;
    }
    case ENUM_MSG_FLUSH_FILE_RESPOND:
    {
        OpeWeight::getInstance().getBook()->updateFileList(pdu);
        QString strEnterDir = OpeWeight::getInstance().getBook()->enterDir();
        if(!strEnterDir.isEmpty()){
            m_strCurPath = m_strCurPath+"/"+strEnterDir;
        }
        break;
    }
    case ENUM_MSG_DEL_DIR_RESPOND:
    {
        QMessageBox::information(this,"删除文件夹",pdu->caData);
        break;
    }
    case ENUM_MSG_RENAME_FILE_RESPOND:
    {
        QMessageBox::information(this,"重命名文件夹",pdu->caData);
        break;
    }
    case ENUM_MSG_ENTER_DIR_RESPOND:
    {
        OpeWeight::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this,"进入文件",pdu->caData);
        break;
    }
    case ENUM_MSG_UPLOAD_FILE_RESPOND:
    {
        QMessageBox::information(this,"上传文件",pdu->caData);
        break;
    }
    case ENUM_MSG_DEL_FILE_RESPOND:
    {
        QMessageBox::information(this,"删除文件",pdu->caData);
        break;
    }
    case ENUM_MSG_DOWNLOAD_FILE_RESPOND:
    {
        qDebug() << "hello";
        qDebug()  << pdu->caData;
        char caFileName[32] = {'\0'};
        sscanf(pdu->caData,"%s %lld",caFileName,&(OpeWeight::getInstance().getBook()->m_total));
        if(strlen(caFileName) && OpeWeight::getInstance().getBook()->m_total)
        {
            OpeWeight::getInstance().getBook()->setDownLoadFlag(true);
            m_file.setFileName(OpeWeight::getInstance().getBook()->getSaveFilePath());
            if(!m_file.open(QIODevice::WriteOnly)){
                QMessageBox::warning(this,"下载文件","保存路径失败");
            }
        }
        break;
    }
    case ENUM_MSG_SHARE_FILE_RESPOND:
    {
        QMessageBox::information(this,"共享文件",pdu->caData);
        break;
    }
    case ENUM_MSG_SHARE_FILE_NODE:
    {
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        char *pos = strrchr(pPath,'/');
        if(pos != NULL){
            pos++;
            QString strNote = QString("%1 share file->%2 \n Do you accept?").arg(pdu->caData).arg(pos);
            int ret = QMessageBox::question(this,"共享文件",strNote);
            if(QMessageBox::Yes == ret){
                PDU *respdu = mkPDU(pdu->uiMsgLen);
                respdu->uiMesgType = ENUM_MSG_SHARE_FILE_NODE_RESPOND;
                memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                QString strName = TcpClient::getInstance().loginName();
                strcpy(respdu->caData,strName.toStdString().c_str());
                m_tcpSocket.write((char*)respdu,respdu->uiPDULen);

            }
        }
    }
    case ENUM_MSG_MOVE_FILE_RESPOND:
    {
        QMessageBox::information(this,"移动文件",pdu->caData);
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}
    else{
        QByteArray buffer = m_tcpSocket.readAll();
        m_file.write(buffer);
        Book *pBook = OpeWeight::getInstance().getBook();
        pBook->m_iRecv += buffer.size();
        if(pBook->m_total == pBook->m_iRecv){
            m_file.close();
            pBook->m_total=0;
            pBook->m_iRecv = 0;
            pBook->setDownLoadFlag(0);
        }else if(pBook->m_total < pBook->m_iRecv){
            m_file.close();
            pBook->m_total=0;
            pBook->m_iRecv = 0;
            pBook->setDownLoadFlag(0);
            QMessageBox::critical(this,"下载文件","下载文件失败！");
        }

    }
}


void TcpClient::on_pushButton_clicked()
{
//    QString strMsg = ui->lineEdit->text();
//    if(!strMsg.isEmpty())
//    {
//        PDU *pdu = mkPDU(strMsg.size());
//        pdu->uiMesgType = 8888;
//        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());

//        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
//        free(pdu);
//        pdu = NULL;
//    }
//    else
//    {
//        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
//    }

    QString strName = ui->lineEdit->text();
    QString strPwd = ui->lineEdit_2->text();

    if(!strName.isEmpty() && !strPwd.isEmpty()){
        m_strName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMesgType = ENUM_MSG_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"登录失败","用户名密码不能为空！");
    }
}

void TcpClient::on_pushButton_2_clicked()
{
    QString strName = ui->lineEdit->text();
    QString strPwd = ui->lineEdit_2->text();

    if(!strName.isEmpty() && !strPwd.isEmpty()){
        m_strName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMesgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }else{
        QMessageBox::critical(this,"注册失败","用户名密码不能为空！");
    }
}
