#ifndef OPEWEIGHT_H
#define OPEWEIGHT_H

#include <QWidget>
#include <QListWidget>
#include <friend.h>
#include <book.h>
#include <QStackedWidget>

class OpeWeight : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWeight(QWidget *parent = nullptr);
    static OpeWeight &getInstance();
    Friend *getFriend();
    Book *getBook();

signals:

public slots:
private:
    QListWidget *m_pListW;
    Friend *m_pfriend;
    Book *m_pBook;
    QStackedWidget *m_pSW;

};

#endif // OPEWEIGHT_H
