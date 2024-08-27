#include "CountDownLatch.h"

using namespace tiny_muduo;

CountDownLatch::CountDownLatch(int count)
    :condition_(),
    count_(count)
{
}



void CountDownLatch::wait()
{
    while(count_ > 0 )
    {
        condition_.wait();
    }
}

void CountDownLatch::countDown()
{
    --count_;
    if (count_ == 0 )
    {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount()const 
{
    return count_;
}
