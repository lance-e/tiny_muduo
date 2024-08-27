#include "TimerQueue.h"
#include "EventLoop.h"
#include "TimerId.h"
#include "Timer.h"
#include <sys/timerfd.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <string.h>

namespace tiny_muduo
{
namespace net
{
namespace detail
{
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue , 0, sizeof newValue);
    memset(&oldValue , 0, sizeof oldValue);

    newValue.it_interval = howMuchTimeFromNow(expiration);

}

}
}
}

using namespace tiny_muduo;
using namespace tiny_muduo::net;
using namespace tiny_muduo::net::detail;


TimerQueue::TimerQueue(EventLoop* loop)
    :loop_(loop),
    timerfd_(timerfd_create(CLOCK_MONOTONIC , TFD_CLOEXEC | TFD_NONBLOCK)),
    timerfdChannel_(loop,timerfd_),
    timers_()
{
    timerfdChannel_.setReadCallBack(std::bind(&TimerQueue::handleRead , this ));
    timerfdChannel_.enableReading();
}


TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    close(timerfd_);
    for (const Entry& timer : timers_)
    {
        delete timer.second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb , Timestamp when , double interval)
{
    Timer* timer = new Timer(std::move(cb) , when , interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop , this , timer));
    return TimerId(timer , timer->sequence());
}


void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged)
    {
        resetTimerfd(timerfd_ , timer->expiration());
    }
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());

    //read timer fd
    uint64_t howmany;
    ssize_t n = read(timerfd_ , &howmany , sizeof howmany);
    std::cout<<"TimerQueue::handleRead() "<< howmany << now.toString() <<std::endl;
    if (n != sizeof howmany)
    {
        printf("TimerQueue::handleRead() reads %ld bytes instead of 8\n" , n);
    } 
}

std::vector<TimerQueue::Entry>  TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now , reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin() , it , back_inserter(expired));
    timers_.erase(timers_.begin() , it);

    return expired;
}


bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

