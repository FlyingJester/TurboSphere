#pragma once
#ifndef LIBFJNET_INTERNAL
#error Do not include this file! Only use socket.h!
#endif

#if (defined USE_CYGSOCK) || (defined USE_BSDSOCK)
typedef int FJNET_SOCKET;
#elif defined USE_WINSOCK
#include <WinSock2.h>
typedef SOCKET FJNET_SOCKET;
#endif

struct WSocket{
    char hostname[0xFF];
    struct sockaddr_in *sockaddr;
    struct hostent *host;
    FJNET_SOCKET sock;
};

#define NANO_IN_MICRO 1000
#define MICRO_IN_MILLI 1000
#define NANO_IN_MILLI (NANO_IN_MICRO*MICRO_IN_MILLI)
#define MILLI_IN_SECOND 1000
#define MICRO_IN_SECOND (MICRO_IN_MILLI*MILLI_IN_SECOND)
#define NANO_IN_SECOND (NANO_IN_MILLI*MILLI_IN_SECOND)

#define MS_INTO_TIMESPEC_P(TIME, MS)\
  TIME->tv_sec = MS/MILLI_IN_SECOND;\
  TIME->tv_usec = (MS*MICRO_IN_MILLI)%MICRO_IN_SECOND

#define MS_INTO_TIMESPEC(TIME, MS)\
  TIME.tv_sec = MS/MILLI_IN_SECOND;\
  TIME.tv_nsec = (MS*MICRO_IN_MILLI)%MICRO_IN_SECOND

#define MS_INTO_TIMEVAL_P(TIME, MS)\
  TIME->tv_sec = MS/MILLI_IN_SECOND;\
  TIME->tv_usec = (MS*NANO_IN_MILLI)%NANO_IN_SECOND

#define MS_INTO_TIMEVAL(TIME, MS)\
  TIME.tv_sec = MS/MILLI_IN_SECOND;\
  TIME.tv_usec = (MS*NANO_IN_MILLI)%NANO_IN_SECOND
