#include "opeweight.h"

OpeWeight::OpeWeight(QWidget *parent) : QWidget(parent)
{
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");
    m_pfriend = new Friend;
    m_pBook = new Book;
    m_pSW = new QStackedWidget;

    m_pSW->addWidget(m_pfriend);
    m_pSW->addWidget(m_pBook);
    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);;
    pMain->addWidget(m_pSW);

    setLayout(pMain);
    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));




}

OpeWeight &OpeWeight::getInstance()
{
    static OpeWeight instance;
    return instance;

}

Friend *OpeWeight::getFriend()
{
    return m_pfriend;

}

Book *OpeWeight::getBook()
{
    return m_pBook;
}
