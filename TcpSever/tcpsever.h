#ifndef TCPSEVER_H
#define TCPSEVER_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

namespace Ui {
class TcpSever;
}

class TcpSever : public QWidget
{
    Q_OBJECT

public:
    explicit TcpSever(QWidget *parent = 0);
    ~TcpSever();
    void loadConfig();

private:
    Ui::TcpSever *ui;
    QString m_strIP;
    quint16 m_usPort;


};

#endif // TCPSEVER_H
