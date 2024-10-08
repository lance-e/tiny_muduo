#include "Poller.h"
#include "EventLoop.h"
#include <poll.h>
#include <assert.h>
#include "Channel.h"
#include "../base/Types.h"

using namespace tiny_muduo;
using namespace net;

Poller::Poller(EventLoop* loop)
    :ownerLoop_(loop)
{
}

Poller::~Poller()
{
}


Timestamp Poller::poll(int timeoutMs , ChannelList* activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin() , pollfds_.size() , timeoutMs);
    Timestamp now(Timestamp::now());
    
    if(numEvents > 0 ){
        printf("%d events happended\n" , numEvents);
        fillActiveChannels(numEvents , activeChannels);
    }else if (numEvents == 0 ){
        printf("nothing happended\n");
    }else {
        printf("Poller::poll() error\n");
    }
    return now;
}

// find event fd's channel , and fill this channel into active channel
void Poller::fillActiveChannels(int numEvents , ChannelList* activeChannels) const
{
    for (PollFdList::const_iterator pfd = pollfds_.begin() ; pfd != pollfds_.end() && numEvents > 0 ; ++pfd)
    {
        if (pfd->revents > 0 )
        {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel* chan = ch->second;
            assert( chan->fd() == pfd->fd);
            chan->set_revents(pfd->revents);
            activeChannels->push_back(chan);
        }
    }
}

void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    printf("fd= %d events= %d \n", channel->fd() , channel->events()); 
    if (channel->index() < 0 ){
        //new one , add pollfds_
        assert(channels_.find(channel->fd() ) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size() - 1);
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    }else {
        //update pollfds_
        assert(channels_.find(channel->fd() ) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
        struct pollfd& pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()){
            pfd.fd =-channel->fd()  -1;
        }
    }
}

void Poller::removeChannel(Channel* channel)
{
  assertInLoopThread();
  printf("fd= %d\n", channel->fd() ); 
  assert(channels_.find(channel->fd()) != channels_.end());
  assert(channels_[channel->fd()] == channel);
  assert(channel->isNoneEvent());
  int idx = channel->index();
  assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
  const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
  assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
  size_t n = channels_.erase(channel->fd());
  assert(n == 1); (void)n;
  if (implicit_cast<size_t>(idx) == pollfds_.size()-1) {
    pollfds_.pop_back();
  } else {
    int channelAtEnd = pollfds_.back().fd;
    iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd-1;
    }
    channels_[channelAtEnd]->set_index(idx);
    pollfds_.pop_back();
  }
}
