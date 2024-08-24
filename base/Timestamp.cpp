#include "Timestamp.h"
#include <sys/time.h>

using namespace tiny_muduo;

Timestamp Timestamp::now()
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
