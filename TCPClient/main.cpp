#include "tcpclient.h"
#include <QApplication>
#include <opeweight.h>
#include "online.h"
#include "friend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TcpClient w;
//    w.show();
//    OpeWeight w;
//    w.show();
//    Online w;
//    w.show();
//    Friend w;
//    w.show();
    TcpClient::getInstance().show();


    return a.exec();
}
