#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

using namespace tiny_muduo;
using namespace tiny_muduo::net;

Acceptor::Acceptor(EventLoop* loop , const InetAddress & listenAddr , bool reuseport)
    :loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family() )),
    acceptChannel_(loop_ , acceptSocket_.fd() ),
    listening_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallBack(std::bind(&Acceptor::handleRead , this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    //acceptChannel_.remove();
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);

    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0 )
    {
        if (newConnectionCallback_){
            newConnectionCallback_(connfd , peerAddr);
        }else {
            close(connfd);
        }
    }
}

