#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <user.hpp>

using namespace muduo;

// 静态方法获取单例对象
ChatService *ChatService::instance()
{
    static ChatService instance;
    return &instance;
}

MsgHandler ChatService::getHandler(int msgid)
{

    auto it = _msgHandlerMap.find(static_cast<EnMsgType>(msgid));
    if (it != _msgHandlerMap.end())
    {
        // 如果直接用_msgHandlerMap[msgid]，如果msgid没有对应的处理器，会自动添加一个空的处理器
        return it->second;
    }
    return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
    {
        LOG_ERROR << "msgid:" << msgid << " can not find handler!";
    };
}

ChatService::ChatService()
{
    // 注册消息以及对应的Handler
    _msgHandlerMap.insert({EnMsgType::LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({EnMsgType::REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do login service!!!";
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    std::string name = js["name"];
    std::string password = js["password"];
    User user;
    user.setName(name);
    user.setPassword(password);

    if (_userModel.insert(user))
    {
        // 注册成功
        json response;
        response["msgId"] = EnMsgType::REG_MSG_ACK;
        response["id"] = user.getId();
        response["errorno"] = 0;
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgId"] = EnMsgType::REG_MSG_ACK;
        response["errorno"] = 1;
        conn->send(response.dump());
    }
}
