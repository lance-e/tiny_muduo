#include "base/CurrentThread.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include <sys/timerfd.h>
#include "net/Channel.h"
#include <cstdio>
#include <unistd.h>

void runInThread()
{
    printf("runInThread():pid :%d , tid = %d\n" ,getpid() , tiny_muduo::CurrentThread::tid());
}

int main()
{
    printf("main(): pid : %d , tid = %d\n" , getpid() , tiny_muduo::CurrentThread::tid());

    tiny_muduo::net::EventLoopThread loopThread;
    tiny_muduo::net::EventLoop* loop = loopThread.startLoop();

    printf("runInLoop\n");
    loop->runInLoop(runInThread);
    sleep(1);

    printf("runAfter\n");
    loop->runAfter(2 , runInThread);


    printf("exit main()\n");

}
