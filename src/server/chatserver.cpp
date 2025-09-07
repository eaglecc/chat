#include "chatserver.hpp"
#include "chatservice.hpp"
#include "public.hpp"
#include "json.hpp"

#include <functional>
#include <string>

using namespace std::placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 1. 注册连接相关的回调函数设置
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    // 2. 注册消息相关的回调函数设置
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    // 3. 设置线程数量4：1个I/O线程，3个工作线程
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开连接
    if (!conn->connected())
    {
        conn->shutdown(); // 关闭连接，释放socketfd资源
        // _loop->quit(); // 退出事件循环
    }
    
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf); // 反序列化
    // 通过js["msgid"]获取消息ID，进行业务分发
    ChatService::instance()->getHandler(static_cast<EnMsgType>(js["msgid"]))(conn, js, time);

}
