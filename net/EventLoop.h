#ifndef __NET_EVENTLOOP_H
#define __NET_EVENTLOOP_H
#include <sys/types.h>
#include "../base/noncopyable.h"
#include "../base/CurrentThread.h"

namespace tiny_muduo
{
namespace net
{

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoop(){
        if (!isInLoopThread()){
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread(){ return threadId_ == tiny_muduo::CurrentThread::tid();}
    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    bool looping_;      //atomic
    const pid_t threadId_;
};

}
}
#endif
