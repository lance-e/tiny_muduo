#ifndef __BASE_ATOMIC_H
#define __BASE_ATOMIC_H
#include "noncopyable.h"
#include <stdint.h>
namespace tiny_muduo
{
namespace detail
{
template <typename T>
class AtomicIntegerT : noncopyable
{
public:
    AtomicIntegerT():value_(0){}
    
    T get()
    {
        return __sync_val_compare_and_swap(&value_ , 0 , 0);
    }

    T incrementAndGet()
    {
        return __sync_fetch_and_add(&value_ , 1);
    }

private:
    volatile T value_;
};
}

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;
}

#endif

