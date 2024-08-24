#ifndef __BASE_TIMESTAMP_H
#define __BASE_TIMESTAMP_H
#include "copyable.h"
#include <boost/operators.hpp>
namespace tiny_muduo
{
class Timestamp : public tiny_muduo::copyable,
                  public boost::equality_comparable<Timestamp>,
                  public boost::less_than_comparable<Timestamp>
{
public:
    Timestamp()
        :microSecondsSinceEpoch_(0)
    {}

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        :microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {}
    static Timestamp now();

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};
}

#endif
