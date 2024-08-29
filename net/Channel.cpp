#include "Channel.h"
#include <poll.h>
#include <stdio.h>
#include "EventLoop.h"
#include <assert.h>

using namespace tiny_muduo;
using namespace net;

const int Channel::kNnoneEvent = 0 ;
const int Channel::kReadEvent = POLLIN | POLLPRI ;
const int Channel::kWriteEvent =POLLOUT  ;

Channel::Channel(EventLoop* loop , int fdArg)
    :loop_(loop),
    fd_(fdArg),
    events_(0),
    revents_(0),
    index_(-1),
    eventHandling_(false)
{
}

Channel::~Channel()
{
    assert(!eventHandling_);
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    loop_->removeChannel(this);
}
    

void Channel::handleEvent()
{
    eventHandling_  = true;

    if (revents_ & POLLNVAL) {
        printf("Channel :: handleEvent() POLLNVAL\n");
    }

    if ((revents_ & POLLHUP ) && !(revents_ &  POLLIN)){
        printf("Channel::handleEvent() POLLHUP\n");
        if (closeCallBack_) closeCallBack_();
    }
    if (revents_ & (POLLERR | POLLNVAL) ){
        if (errorCallBack_) errorCallBack_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if (readCallBack_) readCallBack_();
    }
    if (revents_ & (POLLOUT)){
        if (writeCallBack_) writeCallBack_();
    }

    eventHandling_ = false;
}


