#include "net/EventLoop.h"
#include <sys/timerfd.h>
#include "net/Channel.h"
#include <string.h>
#include <cstdio>
#include <unistd.h>

tiny_muduo::net::EventLoop* g_loop;

void timeout()
{
    printf("Timeout\n");
    g_loop->quit();
}

int main()
{

    tiny_muduo::net::EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC  , TFD_NONBLOCK | TFD_CLOEXEC );
    tiny_muduo::net::Channel channel(&loop , timerfd);
    channel.setReadCallBack(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong , sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd , 0 , & howlong , NULL);

    loop.loop();

    ::close(timerfd);
}
