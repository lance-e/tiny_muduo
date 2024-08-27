#ifndef __NET_TIMERID_H
#define __NET_TIMERID_H
#include "../base/copyable.h"
#include <cstdlib>

namespace tiny_muduo
{
namespace net
{

class Timer;

class TimerId : public copyable 
{
public:
    TimerId()
        :timer_(NULL),
        sequence_(0)
    {}

    TimerId(Timer* timer , int64_t seq)
        :timer_(timer),
        sequence_(seq)
    {}

    friend class TimerQueue;
private:
    Timer* timer_;
    int64_t sequence_;
};

}
}

#endif
