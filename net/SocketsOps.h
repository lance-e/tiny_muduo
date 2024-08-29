#ifndef __NET_SOCKETSOPS_H
#define __NET_SOCKETSOPS_H
#include <sys/socket.h>
#include <arpa/inet.h>
namespace tiny_muduo
{
namespace net
{
namespace sockets
{
///
/// Creates a non-blocking socket file descriptor,
/// abort if any error.
int createNonblockingOrDie(sa_family_t family);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
void toIpPort(char* buf, size_t size,
              const struct sockaddr* addr);
void toIp(char* buf, size_t size,
          const struct sockaddr* addr);

void toHostPort(char* buf, size_t size,const struct sockaddr_in& addr);

struct sockaddr_in6 getLocalAddr(int sockfd);


}
}
}

#endif
