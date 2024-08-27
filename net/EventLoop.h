#ifndef __NET_EVENTLOOP_H
#define __NET_EVENTLOOP_H
#include <sys/types.h>
#include <vector>
#include "../base/noncopyable.h"
#include "../base/CurrentThread.h"
#include <boost/scoped_ptr.hpp>
#include "../base/Timestamp.h"
#include "Callbacks.h"
#include "TimerId.h"

namespace tiny_muduo
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    //(the key of thread safe)
    //run the callback 
    void runInLoop(Functor cb);

    void queueInLoop(Functor cb);

    TimerId runAt(const Timestamp& time , const TimerCallback& cb);
    TimerId runAfter(double delay, const TimerCallback& cb);
    TimerId runEvery(double interval , const TimerCallback& cb);


    void assertInLoopThread(){
        if (!isInLoopThread()){
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread(){ 
        return threadId_ == CurrentThread::tid();
        
    }
    static EventLoop* getEventLoopOfCurrentThread();


    void wakeup();
    void updateChannel(Channel* channel);
private:

    void abortNotInLoopThread();
    void handleRead();      //wake up
    void doPendingFunctors();

    typedef std::vector<Channel*> ChannelList;

    bool looping_;      //atomic
    bool quit_;         //atomic
    bool callingPendingFunctors_;   //atomic

    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    //don't expose channel to client
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    pthread_mutex_t mutex_;
    std::vector<Functor> PendingFunctors_;   //guarded by mutex
};

}
}
#endif
