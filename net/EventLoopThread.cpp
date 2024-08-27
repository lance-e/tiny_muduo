#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace tiny_muduo;
using namespace tiny_muduo::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb , const string& name)
    :loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc , this) ,name ),
    condition_(),
    callback_(cb)
{
    pthread_mutex_init(&mutex_ , NULL);
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    pthread_mutex_destroy(&mutex_);
    if (loop_ != NULL) 
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    EventLoop* loop = NULL;
    {
        //pthread_mutex_lock(&mutex_);
        while ( loop_ == NULL)
        {
            condition_.wait();
        }
        loop = loop_;
        //pthread_mutex_unlock(&mutex_);
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if (callback_)
    {
        callback_(&loop);
    }
    {
        pthread_mutex_lock(&mutex_);
        loop_ = &loop;
        condition_.notify();
        pthread_mutex_unlock(&mutex_);
    }

    loop.loop();

}
