#ifndef __BASE_TYPES_H
#define __BASE_TYPES_H
#include <string>
namespace tiny_muduo
{
using std::string;

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}
}


#endif
