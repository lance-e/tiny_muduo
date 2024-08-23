#ifndef __BASE_THREAD_H
#define __BASE_THREAD_H
#include "sys/types.h"
#include "sys/syscall.h"
#include <functional>
#include <unistd.h>
#include "Types.h"
#include <pthread.h>
#include "Atomic.h"
#include "CountDownLatch.h"

namespace tiny_muduo
{
class Thread
{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(ThreadFunc , const string& name = string());
    ~Thread();

    void start();
    int join();

    bool started()const { return started_;}
    pid_t tid()const {return tid_;}

    const string& name() const {return name_;}
    static int numCreated(){ return nameCreated_.get();}

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    string name_;
    CountDownLatch latch_;
    
    static AtomicInt32 nameCreated_;

};
}

#endif
