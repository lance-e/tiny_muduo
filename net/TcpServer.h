#ifndef __NET_TCPSERVER_H
#define __NET_TCPSERVER_H
#include "../base/noncopyable.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <map>
#include "TcpConnection.h"
namespace tiny_muduo
{
namespace net
{

class Acceptor;
class EventLoop;

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop* loop , const InetAddress& listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    {connectionCallback_ = cb;}

    void setMessageCallback(const MessageCallback& cb)
    {messageCallback_ = cb ;}
private:
    void newConnection(int sockfd , const InetAddress& peerAddr);
    void removeConnection( const TcpConnectionPtr& conn);

    typedef std::map<std::string , TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;   //the acceptor loop
    const std::string name_;
    boost::scoped_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool started_;
    int nextConnId_;        //always in loop thread
    ConnectionMap connections_; //map of connections

};
}

}

#endif
