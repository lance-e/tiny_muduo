#include "EventLoop.h"
#include <assert.h>
#include "../base/CurrentThread.h"
#include "cstdio"

using namespace tiny_muduo;
using namespace tiny_muduo::net;

__thread EventLoop* t_loopInThisThread = 0;


EventLoop::EventLoop()
    :looping_(false),
    threadId_(tiny_muduo::CurrentThread::tid())
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
    assertInLoop();

    looping_ = true;

    // handling 

    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    printf("EventLoop::abortNotInLoopThread : this EventLoop was create in thread:%d , current is %d \n  " , threadId_ , tiny_muduo::CurrentThread::tid());
}
