#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

using namespace std;

void resetHandler(int)
{
    ChatService::instance()->reset(); // 重置服务对象
    exit(0); // 退出程序
}

int main()
{
    signal(SIGINT, resetHandler); // 处理ctrl+c信号

    EventLoop loop; // 创建事件循环对象
    InetAddress listenAddr("127.0.0.1",8888); // 设置服务器监听端口
    ChatServer server(&loop, listenAddr, "ChatServer"); // 创建服务器对象
    server.start(); // 启动服务器
    loop.loop(); // 启动事件循环
    return 0;
}