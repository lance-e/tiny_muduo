#include "EventLoop.h"
#include <assert.h>
#include "../base/CurrentThread.h"
#include "cstdio"
#include "Poller.h"
#include "Channel.h"

using namespace tiny_muduo;
using namespace tiny_muduo::net;

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop()
    :looping_(false),
    quit_(false),
    threadId_(tiny_muduo::CurrentThread::tid()),
    poller_(new Poller(this)),
    activeChannels_(0)
{
    printf("in thread : %d\n" , threadId_);
    if (t_loopInThisThread ){
        printf("another event loop exist in this thread %d\n" , threadId_);
        return ;
    }else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop(){
    assert(!looping_);    
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
        poller_->poll(kPollTimeMs , &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin() ; it != activeChannels_.end() ;++it)
        {
            (*it)->handleEvent();
        }
    }

    printf("EventLoop stop looping\n");

    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    printf("EventLoop::abortNotInLoopThread : this EventLoop was create in thread:%d , current is %d \n  " , threadId_ , tiny_muduo::CurrentThread::tid());
}

void EventLoop::quit()
{
    quit_ = true;
}


void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}
