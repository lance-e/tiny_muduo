#ifndef __BASE_NONCOPYABLE_H
#define __BASE_NONCOPYABLE_H
namespace tiny_muduo
{
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;


protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
}
#endif

