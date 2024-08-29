#include "net/Acceptor.h"
#include "net/Callbacks.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/InetAddress.h"
#include <sys/timerfd.h>
#include "net/Channel.h"
#include <cstdio>
#include <unistd.h>
#include "net/TcpConnection.h"
#include "net/TcpServer.h"

void onConnection(const tiny_muduo::net::TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        printf("onConnection() : new connection [%s] from %s\n" , conn->name().c_str() , conn->peerAddress().toHostPort().c_str());
    }
    else 
    {
        printf("onConnection(): connection [%s] is down\n" , conn->name().c_str());
    }
}

void onMessage(const tiny_muduo::net::TcpConnectionPtr& conn , const char* data , ssize_t len)
{
    printf("onMessage(): receive %zd bytes from connection [%s]\n" , len , conn->name().c_str());
}

int main()
{
    printf("main() : pid %d , \n"  , getpid());
    tiny_muduo::net::InetAddress listenAddr(9981);

    tiny_muduo::net::EventLoop loop;
    tiny_muduo::net::TcpServer server(&loop , listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();

    loop.loop();

}
