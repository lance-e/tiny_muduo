#include "Socket.h"
#include "SocketsOps.h"
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdio.h>
#include "InetAddress.h"
#include <sys/socket.h>

using namespace tiny_muduo;
using namespace tiny_muduo::net;



Socket::~Socket()
{
  close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info* tcpi) const
{
  socklen_t len = sizeof(*tcpi);
  memset(tcpi,0 ,  len);
  return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char* buf, int len) const
{
  struct tcp_info tcpi;
  bool ok = getTcpInfo(&tcpi);
  if (ok)
  {
    snprintf(buf, len, "unrecovered=%u "
             "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
             "lost=%u retrans=%u rtt=%u rttvar=%u "
             "sshthresh=%u cwnd=%u total_retrans=%u",
             tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
             tcpi.tcpi_rto,          // Retransmit timeout in usec
             tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
             tcpi.tcpi_snd_mss,
             tcpi.tcpi_rcv_mss,
             tcpi.tcpi_lost,         // Lost packets
             tcpi.tcpi_retrans,      // Retransmitted packets out
             tcpi.tcpi_rtt,          // Smoothed round trip time in usec
             tcpi.tcpi_rttvar,       // Medium deviation
             tcpi.tcpi_snd_ssthresh,
             tcpi.tcpi_snd_cwnd,
             tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
  }
  return ok;
}

void Socket::bindAddress(const InetAddress& addr)
{
    int ret = ::bind(sockfd_ ,addr.getSockAddr() , static_cast<socklen_t>(sizeof(struct sockaddr_in6))); 
    if ( ret < 0)
    {
        printf("Socket::bindAddress error\n");
    }
}

void Socket::listen()
{
    int ret = ::listen(sockfd_  , SOMAXCONN) ;
    if (ret < 0 )
    {
        printf("Socket::listen error\n");
    }
}

int Socket::accept(InetAddress* peeraddr)
{
  struct sockaddr_in6 addr;
  memset(&addr, 0 ,  sizeof addr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
#if VALGRIND || defined (NO_ACCEPT4)
  int connfd = ::accept(sockfd_, sockaddr_cast(&addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
  int connfd = ::accept4(sockfd_, sockets::sockaddr_cast(&addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
  if(connfd < 0)
  {
    int savedErrno = errno;
    printf( "Socket::accept\n");
    switch (savedErrno)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: // ???
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        printf("unexpected error of ::accept %d\n ", savedErrno);
        break;
      default:
        printf( "unknown error of ::accept %d\n" , savedErrno);
        break;  
    }
  }
  else 
  {
    peeraddr->setSockAddrInet6(addr);
  }
  return connfd;
}

void Socket::shutdownWrite()
{
  if (::shutdown(sockfd_ , SHUT_WR) < 0 )
  {
      printf("shutdown error\n");
  }
}

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
    printf("SO_REUSEPORT failed.\n");
  }
#else
  if (on)
  {
    printf("SO_REUSEPORT is not supported.\n");
  }
#endif
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

