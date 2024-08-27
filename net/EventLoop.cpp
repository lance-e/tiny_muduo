#include "EventLoop.h"
#include <assert.h>
#include "../base/CurrentThread.h"
#include "cstdio"
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"
#include <sys/eventfd.h>

using namespace tiny_muduo;
using namespace tiny_muduo::net;

namespace 
{
int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    printf("Failed in eventfd\n");
    abort();
  }
  return evtfd;
}
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;
}
EventLoop::EventLoop()
    :looping_(false),
    quit_(false),
    threadId_(CurrentThread::tid()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this , wakeupFd_)),
    activeChannels_(0)
{
    printf("in thread : %d\n" , threadId_);
    if (t_loopInThisThread ){
        printf("another event loop exist in this thread %d\n" , threadId_);
        abort();
    }else {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead , this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop(){
    assert(!looping_);    
    wakeupChannel_->disableAll();
    //wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();

    looping_ = true;
    quit_ = false;

    // handling 
    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs , &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin() ; it != activeChannels_.end() ;++it)
        {
            (*it)->handleEvent();
        }
        doPendingFunctors();
    }
    printf("EventLoop stop looping\n");
    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    printf("EventLoop::abortNotInLoopThread : this EventLoop was create in thread:%d , current is %d \n  " , threadId_ , tiny_muduo::CurrentThread::tid());
    abort();
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

//run the callback
void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        printf("run in loop thread\n");
        cb();
    }
    else 
    {
        printf("queue in loop\n");
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        //pthread_mutex_lock(&mutex_);
        PendingFunctors_.push_back(cb);
        //pthread_mutex_unlock(&mutex_);
    }
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}


TimerId EventLoop::runAt(const Timestamp& time ,const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb , time , 0.0);
}

TimerId EventLoop::runAfter(double delay , const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now() , delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval , const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now() , interval));
    return timerQueue_->addTimer(cb , time , interval);
}

//write date into wakeupFd_
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_ , &one , sizeof one);
    if (n != sizeof one)
    {
        printf("EventLoop:;wakeup() writes %ld bytes instead of 8\n" , n);
    }
}

//read date from wakeupFd_
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_ , &one , sizeof one);
    if (n != sizeof one)
    {
        printf("EventLoop::handleRead() reads %ld bytes instead of 8\n" , n);
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}


void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        pthread_mutex_lock(&mutex_);
        functors.swap(PendingFunctors_);        //key : swap into local value 
        pthread_mutex_unlock(&mutex_);
    }

    for (size_t i  = 0  ; i < functors.size() ; i ++)
    {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}
