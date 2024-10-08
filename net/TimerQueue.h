#ifndef __NET_TIMERQUEUE_H
#define __NET_TIMERQUEUE_H
#include "../base/noncopyable.h"
#include "../base/Timestamp.h"
#include "Channel.h"
#include "Callbacks.h"
#include <set>
#include <utility>
#include <vector>
namespace tiny_muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb , 
            Timestamp when,
            double interval);

    //void cancel(TimerId timerId);
    
private:
    typedef std::pair<Timestamp , Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer* , int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired ,Timestamp now);

    bool insert(Timer* timer);

    EventLoop* loop_;
    const int timerfd_;

    Channel timerfdChannel_;

    TimerList  timers_;

    ActiveTimerSet activeTimers_;
};
}
}
#endif
