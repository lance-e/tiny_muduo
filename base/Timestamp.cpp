#include "Timestamp.h"
#include <string>
#include <sys/time.h>
#include <inttypes.h>

using namespace tiny_muduo;

std::string Timestamp::toString() const
{
    char buf[32] = {0};
    snprintf(buf , sizeof(buf) , "%" PRId64 ".%06" PRId64 "", microSecondsSinceEpoch_ / kMicroSecondsPerSecond , microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
