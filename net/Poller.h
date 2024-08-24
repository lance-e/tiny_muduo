#ifndef __NET_POLLER_H
#define __NET_POLLER_H
#include <vector>
#include <map>
#include "../base/noncopyable.h"
#include "EventLoop.h"
#include "../base/Timestamp.h"

struct pollfd;

namespace tiny_muduo
{
namespace net
{

class Channel;

class Poller : noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller();

    Timestamp poll(int timeoutMs , ChannelList* activeChannels);

    void updateChannel(Channel* channel);

    void assertInLoopThread() {ownerLoop_->assertInLoopThread();}

private:
    void fillActiveChannels(int numEvents , ChannelList* activeChannel) const;
    
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int , Channel*> ChannelMap;
    
    EventLoop* ownerLoop_;
    PollFdList pollfds_;        //poll fd cache
    ChannelMap channels_;       //map of fd and channel

};
}
}
#endif
