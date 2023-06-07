#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"
#include "protocol.h"
#include "online.h"
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    static ShareFile &getInstance();
    void updateFriend(QListWidget *pFriendList);

signals:

public slots:
    void cancelSelect();
    void selectAll();
    void okShare();
    void cancelShare();

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCancel;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QButtonGroup *m_pButtonGroup;
    QVBoxLayout *m_pFriendWVBL;
};

#endif // SHAREFILE_H
