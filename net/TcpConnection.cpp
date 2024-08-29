#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include <assert.h>
#include <stdio.h>
#include <memory>

using namespace tiny_muduo;
using namespace tiny_muduo::net;

TcpConnection::TcpConnection(EventLoop* loop , const std::string& name , int sockfd , const InetAddress& localAddr , const InetAddress& peerAddr)
    :loop_(loop),
    name_(name ),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop , sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
    printf("TcpConnection::ctor [%s]\n", name_.c_str());
    channel_->setReadCallBack(std::bind(&TcpConnection::handleRead , this));
    channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite , this));
    channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
    channel_->setWriteCallBack(std::bind(&TcpConnection::handleError, this));

}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread(); 
    assert(state_ == kConnecting);
    setState(kConnected);

    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();
    printf("TcpConnection::connectDestroyed state = %d\n" , state_);
    assert(state_ == kConnected);

    channel_->disableAll();
    connectionCallback_(shared_from_this());

    channel_->remove();
}


void TcpConnection::handleRead()
{
    char buf[65536];
    ssize_t n = ::read(channel_->fd() , buf , sizeof buf);
    if (n > 0 )
    {
        messageCallback_(shared_from_this() , buf , n);
    }
    else if (n == 0 )
    {
        handleClose();
    }
    else 
    {
        errno = 0 ;
        printf("TcpConnection::handleRead error\n");
        handleError();
    }

}

void TcpConnection::handleWrite()
{
}


void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    assert( state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);

    channel_->disableAll();

    //must be the last line
    closeCallback_(shared_from_this());
}


void TcpConnection::handleError()
{
    printf("TcpConnection::handleError : error\n");
}
