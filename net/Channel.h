#ifndef __NET_CHANNEL_H
#define __NET_CHANNEL_H
#include "../base/noncopyable.h"
#include <functional>
namespace tiny_muduo
{
namespace net
{

class EventLoop; 

// Channel:
// A selectable I/O Channel 
// A channel to a file descriptor , but not own this fd 
// A channel belong to a I/O thread(EventLoop)
class Channel : noncopyable
{
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop , int fd);

    void handleEvent();
    void setReadCallBack(const EventCallback& cb)
    { readCallBack_ = cb;}
    void setWriteCallBack(const EventCallback& cb)
    { writeCallBack_ = cb;}
    void setErrorCallBack(const EventCallback& cb)
    { errorCallBack_ = cb;}
    
    int fd() const {return fd_;}
    int events() const { return events_;}
    void set_revents(int rev) {revents_  = rev;}
    bool isNoneEvent() const {return events_ == kNnoneEvent;}

    void enableReading() { events_ |= kReadEvent ; update();}
    //void enableWriting() { events_ |= kWriteEvent; update();}
    //void disableWritingk() { events_ &= ~kReadEvent ; update();}
    //void disableAll() { events_ = kNnoneEvent; update();}

    //for poller
    int index() { return index_;}
    void set_index(int idx) {index_ = idx;}

    EventLoop* ownerLoop() {return loop_;}

private:
    void update();

    static const int kNnoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;


    EventLoop* loop_;
    const int fd_ ;
    int events_;
    int revents_;
    int index_;     //used in poller


    EventCallback readCallBack_;
    EventCallback writeCallBack_;
    EventCallback errorCallBack_;
    
};

}
}

#endif
