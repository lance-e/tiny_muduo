#include "Timer.h"

using namespace tiny_muduo;
using namespace tiny_muduo::net;

AtomicInt64 Timer::s_numCreated;

void Timer::restart(Timestamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now , interval_);
    }
    else 
    {
        expiration_ = Timestamp::invalid();
    }
}
        
