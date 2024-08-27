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

    std::string toString() const;

    int64_t microSecondsSinceEpoch() const
    { return microSecondsSinceEpoch_;}

    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs , Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline Timestamp addTime(Timestamp timestamp , double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif
