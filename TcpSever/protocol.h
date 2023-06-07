#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <string>
#include <unistd.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed:name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:name existed"


#define SEARCG_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offlien"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "user offline"
#define ADD_FRIEND_NO_EXISTED "not exist"

#define DEL_FRIEND "delete OK"

#define DIR_NO_EXIST "cur dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir ok"
#define DEL_DIR_OK "del dir ok"
#define DEL_DIR_FAILURED "del dir no ok"

#define RENAME_FILE_OK "rename ok"
#define RENAME_FILE_FAILED "rename no ok"

#define ENTER_DIR_FAILURED "enter dir failured"

#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILURED "upload file no ok"

#define DEL_FILE_OK "del file ok"
#define DEL_FILE_FAILURED "del file no ok"

#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILURED "move file no ok"
#define COMMON_ERR "system busy"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST, //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复
    ENUM_MSG_LOGIN_REQUEST,  // 登录请求
    ENUM_MSG_LOGIN_RESPOND,  // 登录回复
    ENUM_MSG_ALL_ONLINE_REQUEST, // 在线用户请求
    ENUM_MSG_ALL_ONLINE_RESPOND,  //在线用户回复
    ENUM_MSG_SEARCH_REQUEST,  // 搜索用户请求
    ENUM_MSG_SEARCH_RESPOND,
    ENUM_MSG_ADD_FRIEND_REQUEST, // 添加好友
    ENUM_MSG_ADD_FRIEND_RESPOND,
    ENUM_MSG_ADD_FRIEND_AGGREE, // 同意添加好友
    ENUM_MSG_ADD_FRIEND_REFUSE,
    ENUM_MSG_FLUSH_FRIEND_REQUEST, // 刷新好友
    ENUM_MSG_FLUSH_FRIEND_RESPOND,

    ENUM_MSG_DEL_FRIEND_REQUEST, // 删除好友
    ENUM_MSG_DEL_FRIEND_RESPOND,

    ENUM_MSG_PRIVATE_CHAT_REQUEST, // 私聊
    ENUM_MSG_PRIVATE_CHAT_RESPOND,

    ENUM_MSG_GROUP_CHAT_REQUEST, // 群聊
    ENUM_MSG_GROUP_CHAT_RESPOND,

    ENUM_MSG_CREATE_DIR_REQUEST, // 创建文件夹请求
    ENUM_MSG_CREATE_DIR_RESPOND,

    ENUM_MSG_FLUSH_FILE_REQUEST, // 查看文件请求
    ENUM_MSG_FLUSH_FILE_RESPOND,

    ENUM_MSG_DEL_DIR_REQUEST, // 删除目录
    ENUM_MSG_DEL_DIR_RESPOND,

    ENUM_MSG_RENAME_FILE_REQUEST, // 重名名
    ENUM_MSG_RENAME_FILE_RESPOND,


    ENUM_MSG_ENTER_DIR_REQUEST, // 进入文件夹
    ENUM_MSG_ENTER_DIR_RESPOND,

    ENUM_MSG_UPLOAD_FILE_REQUEST, // 上传文件
    ENUM_MSG_UPLOAD_FILE_RESPOND,

    ENUM_MSG_DEL_FILE_REQUEST, // 删除文件
    ENUM_MSG_DEL_FILE_RESPOND,

    ENUM_MSG_DOWNLOAD_FILE_REQUEST, // 删除文件
    ENUM_MSG_DOWNLOAD_FILE_RESPOND,

    ENUM_MSG_SHARE_FILE_REQUEST, // 删除文件
    ENUM_MSG_SHARE_FILE_RESPOND,
    ENUM_MSG_SHARE_FILE_NODE,
    ENUM_MSG_SHARE_FILE_NODE_RESPOND,


    ENUM_MSG_MOVE_FILE_REQUEST, // 删除文件
    ENUM_MSG_MOVE_FILE_RESPOND,

//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,
//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,


    ENUM_MSG_TYPE_MAX=0x00ffffff
};
struct FileInfo{
    char caName[32];
    int iFileType;

};
struct PDU
{
    uint uiPDULen;  // 总的协议数据单元
    uint uiMesgType; // 消息类型
    char caData[64];
    uint uiMsgLen; // 实际消息长度
    int caMsg[]; //实际消息
};

PDU *mkPDU(uint uiMsgLen);


#endif // PROTOCOL_H
