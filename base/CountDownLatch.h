#ifndef __BASE_COUNTDOWNLATCH_H
#define __BASE_COUNTDOWNLATCH_H
#include "noncopyable.h"
#include "Condition.h"

#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

namespace tiny_muduo
{
class CountDownLatch: noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    Condition condition_ ;
    int count_ ;

};

}

#endif
