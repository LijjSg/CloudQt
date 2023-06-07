#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(pdu == NULL) return;
    uint uisize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for(uint i = 0; i < uisize; i++){
        memcpy(caTmp, (char*)pdu->caMsg+i*32,32);
        ui->listWidget->addItem(caTmp);
    }
}

void Online::on_add_friend_clicked()
{
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    QString strPerUsrName = pItem->text();

    QString strLoginName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMesgType = ENUM_MSG_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
    memcpy(pdu->caData + 32,strLoginName.toStdString().c_str(),strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

