#ifndef __NET_EVENTLOOPTHREAD_H
#define __NET_EVENTLOOPTHREAD_H
#include "../base/noncopyable.h"
#include "../base/Thread.h"
#include "../base/Condition.h"
#include <functional>
#include <string>
namespace tiny_muduo
{
namespace net
{
class EventLoop;

class EventLoopThread : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback() ,
            const std::string & name =  std::string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    pthread_mutex_t mutex_;
    bool exiting_;
    Thread thread_;
    Condition condition_;
    ThreadInitCallback callback_;

};
}
}

#endif
