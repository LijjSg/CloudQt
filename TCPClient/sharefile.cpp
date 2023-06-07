#include "sharefile.h"
#include <QCheckBox>
#include <QDebug>
#include "tcpclient.h"
#include "opeweight.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancelSelectPB = new QPushButton("取消全选");

    m_pOKPB = new QPushButton("确定");
    m_pCancel = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendW = new QWidget;
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancel);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(okShare()));
    connect(m_pCancel,SIGNAL(clicked(bool)),this,SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance ;
    return instance;
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if(pFriendList == NULL){
        return;
    }

   QAbstractButton *tmp = NULL;
   QList<QAbstractButton*> preFriendList =  m_pButtonGroup->buttons();
   for(int i = 0; i < preFriendList.size();i++){
       tmp = preFriendList[i];
       m_pFriendWVBL->removeWidget(tmp);
       m_pButtonGroup->removeButton(tmp);
       preFriendList.removeOne(tmp);
       delete tmp;
       tmp = NULL;
   }
   QCheckBox *pCB = NULL;
   // 刷新好友之后才能显示分享的好友

   for(int i = 0; i < pFriendList->count(); i++){

       pCB = new QCheckBox(pFriendList->item(i)->text());
       m_pFriendWVBL->addWidget(pCB);
       m_pButtonGroup->addButton(pCB);
   }
   m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancelSelect()
{
     QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();
     for(int i = 0; i < cbList.size(); i++){
         if(cbList[i]->isChecked())
         {
             cbList[i]->setChecked(false);
         }
     }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();
    for(int i = 0; i < cbList.size(); i++){
        if(!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare()
{
    QString strName = TcpClient::getInstance().loginName();
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strShareFileName = OpeWeight::getInstance().getBook()->getShareFileName();

    QString strPath = strCurPath + "/" + strShareFileName;

    QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();
    int num = 0;
    for(int i = 0; i < cbList.size(); i++){
        if(cbList[i]->isChecked())
        {
            num++;
        }
    }

    PDU *pdu = mkPDU(32*num + strPath.size()+1);
    pdu->uiMesgType = ENUM_MSG_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);
    int j = 0;
    for(int i = 0; i < num; i++)
    {
        if(cbList[i]->isChecked()){
            memcpy((char*)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),cbList[i]->text().size());
            j++;
        }

    }
    memcpy((char*)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
    hide();
}

void ShareFile::cancelShare()
{
    hide();
}
