#ifndef __NET_EVENTLOOP_H
#define __NET_EVENTLOOP_H
#include <sys/types.h>
#include <vector>
#include "../base/noncopyable.h"
#include "../base/CurrentThread.h"
#include <boost/scoped_ptr.hpp>

namespace tiny_muduo
{
namespace net
{

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void assertInLoopThread(){
        if (!isInLoopThread()){
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread(){ return threadId_ == tiny_muduo::CurrentThread::tid();}
    static EventLoop* getEventLoopOfCurrentThread();


    void updateChannel(Channel* channel);
private:
    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;

    bool looping_;      //atomic
    bool quit_;         //atomic
    const pid_t threadId_;
    boost::scoped_ptr<Poller> poller_;
    ChannelList activeChannels_;

};

}
}
#endif
