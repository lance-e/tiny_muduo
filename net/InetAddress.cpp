#include "InetAddress.h"
#include <netdb.h>
#include <netinet/in.h>
#include "SocketsOps.h"
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <endian.h>

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

using namespace tiny_muduo;
using namespace tiny_muduo::net;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6),
              "InetAddress is same size as sockaddr_in6");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");

InetAddress::InetAddress(uint16_t portArg, bool loopbackOnly, bool ipv6)
{
  static_assert(offsetof(InetAddress, addr6_) == 0, "addr6_ offset 0");
  static_assert(offsetof(InetAddress, addr_) == 0, "addr_ offset 0");
  if (ipv6)
  {
    memset(&addr6_, 0 , sizeof addr6_);
    addr6_.sin6_family = AF_INET6;
    in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
    addr6_.sin6_addr = ip;
    addr6_.sin6_port = htobe16(portArg);
  }
  else
  {
    memset(&addr_,0 , sizeof addr_);
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = htobe32(ip);
    addr_.sin_port = htobe16(portArg);
  }
}

InetAddress::InetAddress(const char* ip, uint16_t portArg, bool ipv6)
{
  if (ipv6 || strchr(ip, ':'))
  {
    memset(&addr6_, 0, sizeof addr6_);
    addr6_.sin6_family  = AF_INET6;
    addr6_.sin6_port = htobe16(portArg);
    if (inet_pton(AF_INET6 , ip , &addr6_.sin6_addr) <= 0)
    {
        printf("error: InetAddress::InetAddress , inet_pton\n");
    }
  }
  else
  {
    memset(&addr_,0 ,  sizeof addr_);
    addr_.sin_family  = AF_INET;
    addr_.sin_port = htobe16(portArg);
    if (inet_pton(AF_INET , ip , &addr_.sin_addr) <= 0)
    {
        printf("error: InetAddress::InetAddress , inet_pton\n");
    }
  }
}

string InetAddress::toIpPort() const
{
  char buf[64] = "";
  sockets::toIpPort(buf, sizeof buf,getSockAddr() );
  return buf;
}

string InetAddress::toIp() const
{
  char buf[64] = "";
  sockets::toIp(buf, sizeof buf, getSockAddr());
  return buf;
}

string InetAddress::toHostPort() const 
{
    char buf[32];
    sockets::toHostPort(buf , sizeof buf , addr_);
    return buf;
}

uint32_t InetAddress::ipv4NetEndian() const
{
  assert(family() == AF_INET);
  return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::port() const
{
  return be16toh(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(const char* hostname, InetAddress* out)
{
  assert(out != NULL);
  struct hostent hent;
  struct hostent* he = NULL;
  int herrno = 0;
  memset(&hent, 0 ,  sizeof(hent));

  int ret = gethostbyname_r(hostname, &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
  if (ret == 0 && he != NULL)
  {
    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return true;
  }
  else
  {
    if (ret)
    {
      printf( "InetAddress::resolve\n");
    }
    return false;
  }
}

void InetAddress::setScopeId(uint32_t scope_id)
{
  if (family() == AF_INET6)
  {
    addr6_.sin6_scope_id = scope_id;
  }
}
