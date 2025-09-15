#ifndef REDIS_HPP
#define REDIS_HPP
#include <hiredis/hiredis.h>
#include <string>
#include <functional>

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器
    bool connect();

    // 向redis指定通道channel发布消息
    bool publish(int channel, const std::string &message);

    // 向redis指定通道subscribe订阅消息
    bool subscribe(int channel);

    // 向redis指定通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 获取订阅的消息
    void observer_channel_message();

    // 设置消息回调函数
    void setMessageCallback(std::function<void(int, std::string)> cb);
}

#endif