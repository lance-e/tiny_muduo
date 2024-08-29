#ifndef __NET_CALLBACKS_H
#define __NET_CALLBACKS_H

#include <functional>
#include <memory>

namespace tiny_muduo
{
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
namespace net
{
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;

// the data has been read to (buf, len)
typedef std::function<void (const TcpConnectionPtr&, const char* data , ssize_t len)> MessageCallback;

typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;

}
}

#endif
