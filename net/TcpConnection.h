#ifndef __NET_TCPCONNECTION_H
#define __NET_TCPCONNECTION_H
#include "../base/noncopyable.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include <memory>
namespace tiny_muduo

{
namespace net
{

class Channel;
class EventLoop;
class Socket;

class TcpConnection :noncopyable,
                    public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop , const std::string& name , int sockfd , const InetAddress& localAddr , const InetAddress& peerAddr);

    ~TcpConnection();

      EventLoop* getLoop() const { return loop_; }
  const std::string& name() const { return name_; }
  const InetAddress& localAddress() { return localAddr_; }
  const InetAddress& peerAddress() { return peerAddr_; }
  bool connected() const { return state_ == kConnected; }

  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

  void setCloseCallback(const CloseCallback& cb)
  { closeCallback_= cb; }


  /// Internal use only.

  // called when TcpServer accepts a new connection
  void connectEstablished();   // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed();  // should be called only once


private:
    enum StateE { kConnecting , kConnected, kDisconnecting , kDisconnected};

    void setState(StateE s ) { state_ = s;}
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

};
}
}

#endif
