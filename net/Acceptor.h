#ifndef __NET_ACCEPTOR_H
#define __NET_ACCEPTOR_H
#include "Socket.h"
#include "Channel.h"
#include "functional"
namespace tiny_muduo
{
namespace net
{
class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    typedef std::function<void(int sockeFd , const InetAddress&) > NewConnectionCallback;

    Acceptor(EventLoop* loop , const InetAddress& listenAddr ,bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb;}

    void listen();

    bool listening() const {return listening_;}


private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    //int idleFd_;

};
}
}
    


#endif
