#ifndef __BASE_CURRENTTHREAD_H
#define __BASE_CURRENTTHREAD_H
namespace tiny_muduo
{
namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread const char* t_threadName;
    void catheTid();

    inline int tid()
    {
        if (__builtin_expect(t_cachedTid == 0 , 0 )){
            catheTid();
        }
        return t_cachedTid;
    }
}
}


#endif
