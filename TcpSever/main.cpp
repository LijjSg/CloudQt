#include "tcpsever.h"
#include <QApplication>
#include "opedb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpeDB::getInstance().init();


    TcpSever w;
    w.show();

    return a.exec();
}
