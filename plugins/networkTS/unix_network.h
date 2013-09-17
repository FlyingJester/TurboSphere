#ifndef TS_NETWORK_UNIX_HEAD
#define TS_NETWORK_UNIX_HEAD

#include <cstdlib>


#ifdef HAS_UNISTD
    #include <unistd.h>
#elif defined HAS_UNISTD_SYS
    #include <sys/unistd.h>
#endif

#define MAX_HOSTNAME_SIZE 0x200 //512 characters. No particular reason for this value.


#endif
