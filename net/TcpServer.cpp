#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "SocketsOps.h"
#include "TcpConnection.h"


using namespace tiny_muduo;
using namespace net;

TcpServer::TcpServer(EventLoop* loop , const InetAddress& peerAddr)
    :loop_(loop),
    name_(peerAddr.toHostPort()),
    acceptor_(new Acceptor(loop , peerAddr, false)),
    started_(false),
    nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection , this , _1 ,_2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    if (!started_) started_ = true;

    if (!acceptor_->listening())
    {
        loop_->runInLoop(std::bind(&Acceptor::listen , acceptor_.get()));
    }
}


void TcpServer::newConnection(int sockfd , const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf , sizeof buf , "#%d" , nextConnId_);
    ++nextConnId_;
    std::string connname = name_+buf;
    printf("TcpServer::newConnection [%s] - new connection [%s] from %s\n" , name_.c_str() , connname.c_str() , peerAddr.toHostPort().c_str());

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_ , connname , sockfd , localAddr , peerAddr));

    connections_[connname] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpConnection::connectDestroyed , conn));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    printf("TcpServer::removeConnection [%s] - connection\n" , conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    assert(n == 1) ; (void)n;
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed , conn));
}

