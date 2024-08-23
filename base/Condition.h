#ifndef __BASE_CONDITION_H
#define __BASE_CONDITION_H
#include "noncopyable.h"
#include <pthread.h>
#include <assert.h>


// 
#define MCHECK(ret) ({__typeof__(ret) errnum = (ret);   \
        if (__builtin_expect(errnum != 0,0))   \
        __assert_perror_fail(errnum , __FILE__ , __LINE__ , __func__);})


namespace tiny_muduo
{

class Condition : noncopyable
{
public:
    Condition()
    {
        MCHECK(pthread_mutex_init(&mutex_,NULL));
        MCHECK(pthread_cond_init(&pcond_ ,NULL));
    }

    ~Condition()
    {
        MCHECK(pthread_mutex_destroy(&mutex_));
        MCHECK(pthread_cond_destroy(&pcond_));
    }

    void wait()
    {
        pthread_mutex_lock(&mutex_);
        pthread_cond_wait(&pcond_ , &mutex_);
        pthread_mutex_unlock(&mutex_);
    }

    void notify()
    {
        MCHECK(pthread_cond_signal(&pcond_));
    }

    void notifyAll()
    {
        MCHECK(pthread_cond_broadcast(&pcond_));
    }

private:
    pthread_mutex_t mutex_;
    pthread_cond_t pcond_;
};
 
}

#endif
