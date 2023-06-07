#include "mytcpsocket.h"
#include <QDebug>
#include "opedb.h"
#include <QString>
#include "mytcpserver.h"

#include <QDir>
#include <QFileInfoList>

myTcpSocket::myTcpSocket()
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    m_bUpload = false;
    m_pTimer = new QTimer;
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));

}

QString myTcpSocket::getName()
{
    return m_strName;
}

void myTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();
    QString srcTmp;
    QString destTmp;

    for(int i = 0; i < fileInfoList.size(); i++)
    {
        if(fileInfoList[i].isFile())
        {
            fileInfoList[i].fileName();
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);

        }else if(fileInfoList[i].isDir())
        {
            if(QString(".") == fileInfoList[i].fileName() ||QString("..") == fileInfoList[i].fileName()) continue;
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);
        }
    }
}


void myTcpSocket::recvMsg()
{
    if(!m_bUpload)
    {
    qDebug() << this->bytesAvailable(); // 当前可读的数据
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint),uiPDULen - sizeof(uint));
    PDU *res = mkPDU(0);
    //qDebug() << pdu->uiMesgType << ":" << (char*)pdu->caMsg;
    switch (pdu->uiMesgType) {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);

        res->uiMesgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret){
            strcpy(res->caData,REGIST_OK);
            QDir dir;
            dir.mkdir(QString("./%1").arg(caName));
        }else{
            strcpy(res->caData,REGIST_FAILED);
        }
        write((char*)res,res->uiPDULen);
        free(res);
        res = NULL;

        break;
    }
    case ENUM_MSG_LOGIN_REQUEST:
    {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
        res->uiMesgType = ENUM_MSG_LOGIN_RESPOND;
        if(ret){
            strcpy(res->caData,LOGIN_OK);
            m_strName = caName;

        }else{
            strcpy(res->caData,LOGIN_FAILED);
        }
        write((char*)res,res->uiPDULen);
        free(res);
        res = NULL;

        break;
    }
    case ENUM_MSG_ALL_ONLINE_REQUEST:
    {
        QStringList ret = OpeDB::getInstance().handAllOnline();
        uint uiMsgLen = ret.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMesgType = ENUM_MSG_ALL_ONLINE_RESPOND;
        for(int i = 0; i < ret.size(); i++){
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_SEARCH_REQUEST:
    {
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_SEARCH_RESPOND;
        if(ret == -1){
            strcpy(respdu->caData,SEARCG_USR_NO);
        }else if(ret == 1){
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }else{
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_ADD_FRIEND_REQUEST:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);
        PDU *respdu = NULL;

        if(ret == -1){
            respdu = mkPDU(0);
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOW_ERROR);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

        }else if(ret == 0){
            respdu = mkPDU(0);
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

        }else if(ret == 1){
            MyTcpServer::getInstance().resend(caPerName,pdu);

        }else if(ret == 2){
            respdu = mkPDU(0);
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

        }else if(ret == 3){
            respdu = mkPDU(0);
            respdu->uiMesgType = ENUM_MSG_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NO_EXISTED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        break;
    }
    case ENUM_MSG_ADD_FRIEND_AGGREE:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData+32, 32);
        qDebug() << caName << "__"<< caPerName;
        OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);

        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_ADD_FRIEND_REFUSE:
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData+32, 32);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_FLUSH_FRIEND_REQUEST:
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        QStringList ret = OpeDB::getInstance().handFlushFriend(caName);
        uint uiMsgLen = ret.size() * 32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMesgType = ENUM_MSG_FLUSH_FRIEND_RESPOND;

        for(int i = 0; i < ret.size(); i++){
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_DEL_FRIEND_REQUEST:
    {
        char caSelfName[32] = {'\0'};
        char caFriendName[32] = {'\0'};
        strncpy(caSelfName, pdu->caData, 32);
        strncpy(caFriendName, pdu->caData+32, 32);
        OpeDB::getInstance().handleDelFriend(caSelfName,caFriendName);

        PDU *respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_DEL_FRIEND_RESPOND;
        strcpy(respdu->caData, DEL_FRIEND);

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;


        MyTcpServer::getInstance().resend(caFriendName,pdu);

        break;
    }

    case ENUM_MSG_PRIVATE_CHAT_REQUEST:
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName,pdu->caData+32,32);
        qDebug() << caPerName;
        MyTcpServer::getInstance().resend(caPerName,pdu);
        break;
    }
    case ENUM_MSG_GROUP_CHAT_REQUEST:
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        QStringList onlineFriend = OpeDB::getInstance().handFlushFriend(caName);
        QString tmp;
        for(int i = 0; i < onlineFriend.size(); i++){
            tmp = onlineFriend.at(i);
            MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
        }
        break;
    }
    case ENUM_MSG_CREATE_DIR_REQUEST:
    {
        QDir  dir;
        QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
        bool ret = dir.exists(strCurPath);
        PDU *respdu;
        if(ret){  //当前目录存在
            char caNewDir[32] = {'\0'};
            memcpy(caNewDir,pdu->caData+32,32);
            QString strNewPath = strCurPath + "/" + caNewDir;

            ret = dir.exists(strNewPath);
            if(ret){
                respdu = mkPDU(0);
                respdu->uiMesgType = ENUM_MSG_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,FILE_NAME_EXIST);
            }else{
                dir.mkdir(strNewPath);
                respdu = mkPDU(0);
                respdu->uiMesgType = ENUM_MSG_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,CREATE_DIR_OK);
            }

        }else{
            respdu = mkPDU(0);
            respdu->uiMesgType = ENUM_MSG_CREATE_DIR_RESPOND;
            strcpy(respdu->caData,DIR_NO_EXIST);
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_FLUSH_FILE_REQUEST:
    {
        char *pCurPath = new char[pdu->uiMsgLen];
        memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
        QDir dir(pCurPath);
        QFileInfoList fileInfoList = dir.entryInfoList();
        int iFileCount = fileInfoList.size();
        PDU *respdu = mkPDU(sizeof(FileInfo)*iFileCount);
        respdu->uiMesgType = ENUM_MSG_FLUSH_FILE_RESPOND;
        FileInfo *pFileInfo = NULL;
        QString strFileName;

        for(int i = 0; i < fileInfoList.size();i++){
            qDebug() << fileInfoList[i].fileName() << " " << fileInfoList[i].size();
            pFileInfo = (FileInfo*)(respdu->caMsg) + i;
            strFileName = fileInfoList[i].fileName();
            memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),strFileName.size());
            if(fileInfoList[i].isDir()){
                pFileInfo->iFileType = 0;
            }else {
                pFileInfo->iFileType = 1;
            }
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_DEL_DIR_REQUEST:
    {
        char caName[32] = {'\0'};
        strcpy(caName,pdu->caData);
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caName);

        QFileInfo fileInfo(strPath);
        int ret;
        if(fileInfo.isDir()){
            QDir dir;
            dir.setPath(strPath);
            ret = dir.removeRecursively();
        }else
        {
            ret = false;
        }

        PDU *respdu = NULL;
        if(ret){
            respdu = mkPDU(strlen(DEL_DIR_OK)+1);
            respdu->uiMesgType = ENUM_MSG_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));

        }else
        {
            respdu = mkPDU(strlen(DEL_DIR_FAILURED)+1);
            respdu->uiMesgType = ENUM_MSG_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_RENAME_FILE_REQUEST:
    {
        char caOldName[32] = {'\0'};
        char caNewName[32] = {'\0'};
        strncpy(caOldName, pdu->caData, 32);
        strncpy(caNewName, pdu->caData+32, 32);
        char *path = new char[pdu->uiMsgLen];
        memcpy(path,pdu->caMsg,pdu->uiMsgLen);

        QString strOldPath = QString("%1/%2").arg(path).arg(caOldName);
        QString strNewPath = QString("%1/%2").arg(path).arg(caNewName);

        QDir dir;
        bool ret = dir.rename(strOldPath,strNewPath);
        PDU *respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_RENAME_FILE_RESPOND;

        if(ret){
            strncpy(respdu->caData,RENAME_FILE_OK,strlen(RENAME_FILE_FAILED));
        }else{
            strncpy(respdu->caData,RENAME_FILE_FAILED,strlen(RENAME_FILE_FAILED));
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_ENTER_DIR_REQUEST:
    {
        char caEnterName[32] = {'\0'};

        strncpy(caEnterName, pdu->caData, 32);

        char *path = new char[pdu->uiMsgLen];
        memcpy(path,pdu->caMsg,pdu->uiMsgLen);

        QString strPath = QString("%1/%2").arg(path).arg(caEnterName);

        QFileInfo fileInfo(strPath);
        PDU *respdu;
        if(fileInfo.isDir()){
            QDir dir(strPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            int iFileCount = fileInfoList.size();
            respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMesgType = ENUM_MSG_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo = NULL;
            QString strFileName;

            for(int i = 0; i < fileInfoList.size();i++){
                qDebug() << fileInfoList[i].fileName() << " " << fileInfoList[i].size();
                pFileInfo = (FileInfo*)(respdu->caMsg) + i;
                strFileName = fileInfoList[i].fileName();
                memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),strFileName.size());
                if(fileInfoList[i].isDir()){
                    pFileInfo->iFileType = 0;
                }else {
                    pFileInfo->iFileType = 1;
                }
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else{
            respdu = mkPDU(0);
            respdu->uiMesgType=ENUM_MSG_ENTER_DIR_RESPOND;
            strcpy(respdu->caData,ENTER_DIR_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        break;
    }
    case ENUM_MSG_UPLOAD_FILE_REQUEST:
    {
        char caFileName[32] = {'\0'};
        qint64 filesize = 0;
        sscanf(pdu->caData,"%s %lld",caFileName,&filesize);
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);

        m_file.setFileName(strPath);
        if(m_file.open(QIODevice::WriteOnly))
        {
            m_bUpload = true;
            m_iTotal = filesize;
            m_iRecved = 0;
        }
        break;
    }
    case ENUM_MSG_DEL_FILE_REQUEST:
    {
        char caName[32] = {'\0'};
        strcpy(caName,pdu->caData);
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caName);

        QFileInfo fileInfo(strPath);
        int ret;
        if(!fileInfo.isDir()){
            QDir dir;
            dir.remove(strPath);
        }else
        {
            ret = false;
        }

        PDU *respdu = NULL;
        if(ret){
            respdu = mkPDU(strlen(DEL_FILE_OK)+1);
            respdu->uiMesgType = ENUM_MSG_DEL_FILE_RESPOND;
            memcpy(respdu->caData,DEL_FILE_OK,strlen(DEL_FILE_OK));

        }else
        {
            respdu = mkPDU(strlen(DEL_FILE_FAILURED)+1);
            respdu->uiMesgType = ENUM_MSG_DEL_FILE_RESPOND;
            memcpy(respdu->caData,DEL_FILE_FAILURED,strlen(DEL_FILE_FAILURED));
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_DOWNLOAD_FILE_REQUEST:
    {
        char caFileName[32] = {'\0'};
        strcpy(caFileName,pdu->caData);
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
        delete []pPath;
        pPath = NULL;

        QFileInfo fileInfo(strPath);
        qint64 fileSize = fileInfo.size();
        PDU *respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_DOWNLOAD_FILE_RESPOND;
        sprintf(respdu->caData,"%s %lld",caFileName,fileSize);
        write((char*)respdu,respdu->uiPDULen);
        qDebug() << "发送";
        free(respdu);
        respdu = NULL;
        m_file.setFileName(strPath);
        m_file.open(QIODevice::ReadOnly);
        m_pTimer->start(1000);

    }
    case ENUM_MSG_SHARE_FILE_REQUEST:
    {
        char caSendName[32] = {'\0'};
        int num = 0;
        sscanf(pdu->caData,"%s%d",caSendName,&num);
        int size = num*32;

        PDU *respdu = mkPDU(pdu->uiMsgLen - size);
        respdu->uiMesgType = ENUM_MSG_SHARE_FILE_NODE;
        strcpy(respdu->caData,caSendName);
        memcpy(respdu->caMsg,(char*)(pdu->caMsg) + size,pdu->uiMsgLen - size);

        char caRecvName[32] = {'\0'};

        for(int i = 0; i < num; i++){
            memcpy(caRecvName,(char*)(pdu->caMsg)+i*32,32);
            MyTcpServer::getInstance().resend(caRecvName,respdu);
        }
        free(respdu);
        respdu=NULL;

        respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_SHARE_FILE_RESPOND;
        strcpy(respdu->caData,"share file ok");
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;

        break;
    }

    case ENUM_MSG_SHARE_FILE_NODE_RESPOND:
    {
        char caRecvPath[32] = {'\0'};
        QString StrRecvPath = QString("./%1").arg(pdu->caData);
        QString strShareFilePath = QString("%1").arg((char*)pdu->caMsg);

        int index = strShareFilePath.lastIndexOf('/');
        QString strFileName = strShareFilePath.right(strShareFilePath.size() - index - 1);

        StrRecvPath  = StrRecvPath + '/' + strFileName;

        QFileInfo fileInfo(strShareFilePath);
        if(fileInfo.isFile())
        {
            QFile::copy(strShareFilePath,StrRecvPath);
        }else if(fileInfo.isDir())
        {
            copyDir(strShareFilePath,StrRecvPath);
        }
    }
    case ENUM_MSG_MOVE_FILE_REQUEST:
    {
        char caFileName[32] = {'\0'};
        int srclen =0;
        int destlen = 0;
        sscanf(pdu->caData,"%d%d%s",&srclen,&destlen,caFileName);
        PDU *respdu = mkPDU(0);
        char *pSrcPath = new char[srclen+1];
        char *pDestPath = new char[destlen+33];
        memset(pSrcPath,'\0',srclen+1);
        memset(pDestPath,'\0',destlen+33);
        respdu->uiMesgType = ENUM_MSG_MOVE_FILE_RESPOND;
        memcpy(pSrcPath,pdu->caMsg,srclen);
        memcpy(pDestPath,(char*)(pdu->caMsg) + (srclen+1),destlen);
        QFileInfo fileInfo(pDestPath);
        if(fileInfo.isDir()){
            strcat(pDestPath,"/");
            strcat(pDestPath,caFileName);
            bool ret = QFile::rename(pSrcPath,pDestPath);
            qDebug() << pSrcPath << " :" << pDestPath;
            if(ret)
            {
                strcpy(respdu->caData,MOVE_FILE_OK);
            }else
            {
                strcpy(respdu->caData,COMMON_ERR);
            }
        }else if(fileInfo.isFile()){
            strcpy(respdu->caData,MOVE_FILE_FAILURED);
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;

        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;

}    //qDebug() << caName << " " << caPwd << pdu->uiMesgType;
    else{
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMesgType = ENUM_MSG_UPLOAD_FILE_RESPOND;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        if(m_iTotal <= m_iRecved){
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else if(m_iTotal <= m_iRecved)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
    }
}

void myTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

void myTcpSocket::sendFileToClient()
{
    char *pData = new char[4096];
    qint64 ret = 0;
    while(1){
        ret = m_file.read(pData,4096);
        if(ret > 0 && ret <= 4096){
            write(pData,ret);
        }else if(ret == 0){
            m_file.close();
            break;
        }else{
            qDebug() << " 发送失败";
            m_file.close();
            break;
        }
    }
}

