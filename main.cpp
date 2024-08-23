#include "base/CurrentThread.h"
#include "net/EventLoop.h"
#include "base/Thread.h"

void threadFunc()
{
    printf("threadFunc(): pid %d , tid= %d\n" , getpid() , tiny_muduo::CurrentThread::tid());

    tiny_muduo::net::EventLoop loop;
    loop.loop();
}

int main()
{
    printf("main(): pid %d , tid= %d\n" , getpid() , tiny_muduo::CurrentThread::tid());

    tiny_muduo::net::EventLoop loop;
    tiny_muduo::Thread thread(threadFunc);
    thread.start();

    loop.loop();
    pthread_exit(NULL);
}
