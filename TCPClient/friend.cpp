#include "friend.h"
#include <tcpclient.h>
#include <QInputDialog>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线好友");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSend = new QPushButton("信息发送");
    m_pPrivatePB = new QPushButton("私聊");


    QVBoxLayout *pLeftPBVBL = new QVBoxLayout;
    pLeftPBVBL->addWidget(m_pDelFriendPB);
    pLeftPBVBL->addWidget(m_pFlushFriendPB);
    pLeftPBVBL->addWidget(m_pShowOnlineUsrPB);
    pLeftPBVBL->addWidget(m_pSearchUsrPB);
    pLeftPBVBL->addWidget(m_pPrivatePB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pLeftPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSend);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUsr()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));
    connect(m_pPrivatePB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));
    connect(m_pMsgSend,SIGNAL(clicked(bool)),this,SLOT(groupChat()));


}

void Friend::showAllOnlie(PDU *pdu)
{
    if(pdu == nullptr) return;
    m_pOnline->showUsr(pdu);
}

void Friend::updataFrind(PDU *pdu)
{
    if(pdu == NULL) return;
    m_pFriendListWidget->clear();

    uint uiSize = pdu->uiMsgLen/32;
    char caName[32] = {'\0'};
    for(uint i = 0; i < uiSize; i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg = QString("%1 : %2").arg(pdu->caData).arg((char*)(pdu->caMsg));
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMesgType = ENUM_MSG_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        m_pOnline->show();

    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
   m_strSearchName = QInputDialog::getText(this,"搜索","用户名：");
   if(!m_strSearchName.isEmpty()){
       qDebug() << m_strSearchName;
       PDU *pdu = mkPDU(0);
       memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
       pdu->uiMesgType = ENUM_MSG_SEARCH_REQUEST;
       TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
       free(pdu);
       pdu = NULL;

   }
}

void Friend::flushFriend()
{

    QString strName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMesgType = ENUM_MSG_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    if(NULL != m_pFriendListWidget->currentItem()){
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PDU *pdu = mkPDU(0);
        pdu->uiMesgType = ENUM_MSG_DEL_FRIEND_REQUEST;
        QString strName = TcpClient::getInstance().loginName();

        memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Friend::privateChat()
{
    if(NULL != m_pFriendListWidget->currentItem())
    {
        QString strChatdName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatdName);
        if(PrivateChat::getInstance().isHidden()){
            PrivateChat::getInstance().show();
        }
    }else{
        QMessageBox::warning(this,"私聊","请选择私聊对象");
    }
}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    m_pInputMsgLE->clear();
    if(!strMsg.isEmpty()){
        PDU *pdu = mkPDU(strMsg.size()+1);
        pdu->uiMesgType = ENUM_MSG_GROUP_CHAT_REQUEST;
        QString strName = TcpClient::getInstance().loginName();
        QString str = QString("%1 : %2").arg(strName).arg(strMsg);
        m_pShowMsgTE->append(str);
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strncpy((char*)(pdu->caMsg),strMsg.toStdString().c_str(),strMsg.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    }else{
        QMessageBox::warning(this,"群聊","群聊信息不能为空");
    }
}
