#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <mutex>
#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmode.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;

using json = nlohmann::json;
// 消息处理器类型
using MsgHandler = function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象
    static ChatService *instance();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    // 重置服务，服务器异常退出后，重置用户的状态信息
    void reset();

private:
    ChatService(/* args */);

    // 消息ID和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    mutex _connMutex;

    // 数据操作类对象
    UserModel _userModel;

    // 离线消息业务
    OfflineMsgModel _offlineMsgModel;

    // 存储好友信息
    FriendModel _friendModel;
};


#endif