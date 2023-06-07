#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    //将数据库定义成单例
    static OpeDB& getInstance();
    void init();
    ~OpeDB();
    bool handleRegist(const char *name,const char *pwd);
    bool handleLogin(const char *name,const char *pwd);
    void handleOffline(const char *name);
    QStringList handAllOnline();
    int handleSearchUsr(const char* name);
    int handleAddFriend(const char *pername,const char *name);
    void handleAgreeAddFriend(const char *caPerName,const char *caName);
    QStringList handFlushFriend(const char *name);
    bool handleDelFriend(const char *name,const char *pername);


signals:

public slots:
private:
    QSqlDatabase m_db;

};

#endif // OPEDB_H
