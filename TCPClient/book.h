#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    qint64 m_total;
    qint64 m_iRecv;
    QString getSaveFilePath();
    QString getShareFileName();

signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void reName();

    void enterDir(const QModelIndex index);
    void returnPre();
    void uploadFile();
    void uploadDate();

    void delFile();
    void downLoad();
    void setDownLoadFlag(bool status);

    bool getDownLoad();

    void SharedFile();
    void MoveFile();
    void selectDir();




private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownloadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFilePB;
    QPushButton *m_pSelectPB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;
    QTimer *m_pTimer;

    QString m_strSaveFilePath;
    bool m_bDownLoad;
    QString m_strShareName;
    QPushButton *m_pMovePB;
    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;





};

#endif // BOOK_H
