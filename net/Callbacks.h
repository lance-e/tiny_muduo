#ifndef __NET_CALLBACKS_H
#define __NET_CALLBACKS_H

#include <functional>

namespace tiny_muduo
{

namespace net
{

typedef std::function<void()> TimerCallback;

}
}

#endif
