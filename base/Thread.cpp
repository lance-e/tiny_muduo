#include "Thread.h"
#include "CurrentThread.h"
#include <assert.h>
#include <sys/prctl.h>
#include <exception>

namespace tiny_muduo
{
namespace detail
{
    //get thread id
    pid_t gettid()
    {
        return static_cast<pid_t>(syscall(SYS_gettid));
    }

    //
    struct ThreadData
    {
        typedef Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        string name_;
        pid_t* tid_;
        CountDownLatch* latch_;

        ThreadData(ThreadFunc func, 
                const string& name,
                pid_t* tid,
                CountDownLatch* latch)
            :func_(std::move(func)),
            name_(name),
            tid_(tid),
            latch_(latch)
        {}
        
        void runInThread()
      {
        *tid_ = tiny_muduo::CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        tiny_muduo::CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
        ::prctl(PR_SET_NAME, tiny_muduo::CurrentThread::t_threadName);
        try
        {
          func_();
          tiny_muduo::CurrentThread::t_threadName = "finished";
        }
        catch (const std::exception& ex)
        {
          tiny_muduo::CurrentThread::t_threadName = "crashed";
          fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
          fprintf(stderr, "reason: %s\n", ex.what());
          abort();
        }
        catch (...)
        {
          tiny_muduo::CurrentThread::t_threadName = "crashed";
          fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
          throw; // rethrow
        }
      }
    
        
                
    };

void* startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

}       //end of detail





void CurrentThread::catheTid(){
    if (t_cachedTid == 0 ){
        t_cachedTid = detail::gettid();
    }
}

AtomicInt32 Thread::nameCreated_;

Thread::Thread(ThreadFunc func , const string& n)
    :started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = nameCreated_.incrementAndGet();        

    if (name_.empty())
    {
        char buf[32];
        snprintf(buf , sizeof(buf) , "Thread%d" , num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    started_ = false;
    delete data; // or no delete?
    printf("Failed in pthread_create");
    abort();
  }
  else
  {
    latch_.wait();
    assert(tid_ > 0);
  }
}


int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_ , NULL);
}



}
