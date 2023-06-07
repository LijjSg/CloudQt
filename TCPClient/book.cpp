#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "friend.h"
#include "opeweight.h"
#include <sharefile.h>

Book::Book(QWidget *parent) : QWidget(parent)
{


    m_strEnterDir.clear();
    m_pTimer = new QTimer;
    m_bDownLoad = false;

    m_pBookListW = new QListWidget;
    m_pReturnPB  = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB= new QPushButton("重命名文件");
    m_pFlushFilePB = new QPushButton("刷新文件");
    m_pMovePB = new QPushButton("移动文件");
    m_pSelectPB = new QPushButton("移动目录");
    m_pSelectPB->setEnabled(false);

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownloadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("分享文件");
    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMovePB);
    pFileVBL->addWidget(m_pSelectPB);


    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);
    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushFilePB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool)),this,SLOT(reName()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterDir(QModelIndex)));

    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnPre()));
    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadDate()));
    connect(m_pDelFilePB,SIGNAL(clicked(bool)),this,SLOT(delFile()));
    connect(m_pDownloadPB,SIGNAL(clicked(bool)),this,SLOT(downLoad()));

    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(SharedFile()));
    connect(m_pMovePB,SIGNAL(clicked(bool)),this,SLOT(MoveFile()));
    connect(m_pSelectPB,SIGNAL(clicked(bool)),this,SLOT(selectDir()));
}

void Book::updateFileList(const PDU *pdu)
{
    if(pdu == NULL) return;
    m_pBookListW->clear();
    FileInfo *pFileInfo = NULL;
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    for(int i = 0; i < iCount; i++){
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug() << pFileInfo->caName << " " << pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(pFileInfo->iFileType == 0){
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }else{
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText(pFileInfo->caName);
        m_pBookListW->addItem(pItem);
    }


}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","文件名");
    if(!strNewDir.isEmpty()){
        QString strName = TcpClient::getInstance().loginName();
        QString strCurPath = TcpClient::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMesgType = ENUM_MSG_CREATE_DIR_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }else{
        QMessageBox::warning(this,"新建文件夹","名字不能为空");
    }

}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMesgType = ENUM_MSG_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void Book::delDir()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件！");
    }else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMesgType = ENUM_MSG_DEL_DIR_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::reName()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"重命名文件","请选择要重命名文件！");
    }else{
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名","请输入新的文件名：");

        if(!strNewName.isEmpty()){
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMesgType = ENUM_MSG_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData + 32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());


            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;

        }else{
            QMessageBox::warning(this,"重命名文件","新文件名字不能为空！");
        }

    }
}

void Book::enterDir(const QModelIndex index)
{
    QString strDirName = index.data().toString();
    m_strEnterDir = strDirName;
    QString strCurPath = TcpClient::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMesgType = ENUM_MSG_ENTER_DIR_REQUEST;
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strRootPath = "./"+TcpClient::getInstance().loginName();
    qDebug() << strCurPath << " " << strRootPath;

    if(strCurPath == strRootPath){
        QMessageBox::warning(this,"返回","返回失败！");
    }else{
        int index = strCurPath.lastIndexOf('/');
        strCurPath.remove(index,strCurPath.size()-index);
        TcpClient::getInstance().setCurPath(strCurPath);

        clearEnterDir();
        flushFile();
    }
}

void Book::uploadFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    m_strUploadFilePath = QFileDialog::getOpenFileName();
    if(!m_strUploadFilePath.isEmpty()){
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size(); //获得文件大小

        PDU *pdu = mkPDU(m_strUploadFilePath.size()+1);
        pdu->uiMesgType = ENUM_MSG_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        m_pTimer->start(1000);
    }else{
        QMessageBox::warning(this,"上传文件","上传文件不能为空");
    }
}

void Book::uploadDate()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","上传文件失败");
    }else{
        char *pBuffer = new char[4096];
        qint64 ret = 0;
        while(1){
            ret = file.read(pBuffer,4096);
            if(ret > 0 && ret <= 4096){
                TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
            }else if(ret == 0){
                break;
            }else{
                QMessageBox::warning(this,"上传文件","读取文件失败");
                break;
            }
        }
        delete []pBuffer;
    }
    file.close();
}

void Book::delFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件！");
    }else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMesgType = ENUM_MSG_DEL_FILE_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::downLoad()
{


    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"下载文件","请选择要下载的文件！");
    }else
    {

        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if(strSaveFilePath.isEmpty()){
            QMessageBox::warning(this,"下载文件","请选择文件！");
            m_strSaveFilePath.clear();
        }else{
            m_strSaveFilePath = strSaveFilePath;
        }

        QString strCurPath = TcpClient::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMesgType = ENUM_MSG_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData,strFileName.toStdString().c_str());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

        free(pdu);
        pdu = NULL;
    }
}

void Book::setDownLoadFlag(bool status)
{
    m_bDownLoad = status;
}

bool Book::getDownLoad()
{
    return m_bDownLoad;
}

void Book::SharedFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件！");
        return;
    }else
    {
        m_strShareName = pItem->text();
    }
    Friend *pFriend = OpeWeight::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);
    if(ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }

}

void Book::MoveFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        QString strCurPath = TcpClient::getInstance().curPath();
        m_strMoveFilePath = strCurPath + '/' + m_strMoveFileName;
        m_pSelectPB->setEnabled(true);

    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件！");
    }
}

void Book::selectDir()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();
        QString strCurPath = TcpClient::getInstance().curPath();
        m_strDestDir = strCurPath + '/' + strDestDir;

        int srclen = m_strMoveFilePath.size();
        int destlen = m_strDestDir.size();
        PDU *pdu = mkPDU(srclen+destlen+2);
        pdu->uiMesgType = ENUM_MSG_MOVE_FILE_REQUEST;
        sprintf(pdu->caData,"%d %d %s",srclen,destlen,m_strMoveFileName.toStdString().c_str());

        memcpy(pdu->caMsg,m_strMoveFilePath.toStdString().c_str(),srclen);
        memcpy((char*)(pdu->caMsg)+ srclen+1,m_strDestDir.toStdString().c_str(),destlen);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件！");
    }

    m_pSelectPB->setEnabled(false);
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareName;
}






















