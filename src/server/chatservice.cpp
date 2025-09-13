#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <user.hpp>
#include <vector>
#include "group.hpp"

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
    _msgHandlerMap.insert({EnMsgType::ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({EnMsgType::ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({EnMsgType::CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({EnMsgType::ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({EnMsgType::GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}

// 登录
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    std::string password = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == password)
    {
        if (user.getState() == "online")
        {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgId"] = EnMsgType::LOGIN_MSG_ACK;
            response["errorno"] = 2; // 2表示该用户已经登录
            response["errmsg"] = "该账号已经登录，请重新输入其他账号!";
            conn->send(response.dump());
            return;
        }

        // 记录用户连接信息
        {
            std::lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert({id, conn});
        }

        // 登录成功
        user.setState("online");
        _userModel.updateState(user);

        json response;
        response["msgId"] = EnMsgType::LOGIN_MSG_ACK;
        response["id"] = user.getId();
        response["name"] = user.getName();
        response["errorno"] = 0;

        // 查询该用户是否有离线消息
        std::vector<std::string> offlineMsgs = _offlineMsgModel.query(id);
        if (!offlineMsgs.empty())
        {
            response["offlinemsg"] = offlineMsgs;
            // 读取该用户的离线消息后，删除掉
            _offlineMsgModel.remove(id);
        }

        // 查询用户的好友信息并返回
        auto userLists = _friendModel.query(id);
        if (!userLists.empty())
        {
            std::vector<string> vec2;
            for (auto &user : userLists)
            {
                json js;
                js["id"] = user.getId();
                js["name"] = user.getName();
                js["state"] = user.getState();
                vec2.push_back(js.dump());
            }
            response["friends"] = vec2;
        }
        
        conn->send(response.dump());
    }
    else
    {
        // 登录失败
        json response;
        response["msgId"] = EnMsgType::LOGIN_MSG_ACK;
        response["errorno"] = 1; // 1表示账号或密码错误
        response["errmsg"] = "账号或密码错误!";
        conn->send(response.dump());
    }
}

// 注册
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

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息
            it->second->send(js.dump());

            return;
        }
    }
    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 添加好友
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    auto userid = js["id"].get<int>();
    auto friendid = js["friendid"].get<int>();
    
    // 存储好友信息
    _friendModel.insert(userid, friendid);
}

// 创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    std::string groupName = js["groupname"];
    std::string groupDesc = js["groupdesc"];
    Group group(-1, groupName, groupDesc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        int userid = js["id"].get<int>();
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = _groupModel.queryGroupsUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id: useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群组消息
            it->second->send(js.dump());
        }
        else
        {
            // 存储离线消息
            _offlineMsgModel.insert(id, js.dump());
        }
    }
}

// 客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    // 获取用户ID
    int userId = -1;
    for (const auto &[id, userConn] : _userConnMap)
    {
        if (userConn == conn)
        {
            userId = id;
            break;
        }
    }
    // 从_userConnMap删除用户的连接信息
    if (userId != -1)
    {
        {
            // 用户在线，进行下线处理
            std::lock_guard<std::mutex> lock(_connMutex);
            _userConnMap.erase(userId);
        }
        User user;
        user.setId(userId);
        // 更新用户的状态信息
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 服务器异常退出
void ChatService::reset()
{
    _userModel.resetState();
}
