#include "poll.h"
#include "socket.h"
#include <stdlib.h>
#include <string.h>

#ifdef USE_WINSOCK
#include <Winsock2.h>

#define FD_COPY(OUT, IN)\
memcpy(IN, OUT, sizeof(struct fd_set))

#elif (defined USE_BSDSOCK) || (defined USE_CYGSOCK)
#include <sys/select.h>

#ifdef HAS_STRINGS
#include <strings.h>
#endif

#endif

#define LIBFJNET_INTERNAL
#include "socket_definition.h"

struct SocketSet {fd_set set;int nfds;};

struct SocketSet *GenerateSocketSet(struct WSocket **sockets, unsigned num_sockets){
    unsigned i = 0;
    struct SocketSet *set = malloc(sizeof(struct SocketSet));
    FD_ZERO(&(set->set));
    set->nfds = 0;

    while(i<num_sockets){
        FD_SET(sockets[i]->sock, &(set->set));
        if(sockets[i]->sock>set->nfds) set->nfds = sockets[i]->sock;
    }

    set->nfds++;

    return set;
}


void FreeSocketSet(struct SocketSet *set){
    FD_ZERO(&(set->set));

    free(set);
}


void PokeSet(struct SocketSet *socket_set){

}


void AddToSet(struct WSocket *socket, struct SocketSet *set){
    FD_SET(socket->sock, &(set->set));

    if(socket->sock+1>set->nfds)
      set->nfds = socket->sock+1;
}

void RemoveFromSet(struct WSocket *socket, struct SocketSet *set){
    FD_CLR(socket->sock, &(set->set));
}

void RemoveFromSetAndClose(struct WSocket *socket, struct SocketSet *set){
    RemoveFromSet(socket, set);
    Disconnect_Socket(socket);
}


int  IsPartOfSet(struct WSocket *socket, struct SocketSet *set){
    return FD_ISSET(socket->sock, &(set->set));
}


int PollImmediate(enum WSockType t, struct WSocket **sockets, unsigned num_sockets, unsigned ms_timeout){
    struct SocketSet *set = GenerateSocketSet(sockets, num_sockets);

    int e = PollSet(t, set, ms_timeout);

    FreeSocketSet(set);

    return e;

}


int PollSet(enum WSockType t, struct SocketSet *set, unsigned ms_timeout){
    fd_set setread, setwrite, seterror;
    struct timeval times;

    MS_INTO_TIMEVAL(times, ms_timeout);

    if(t&eRead)
      memcpy(&setread, &(set->set), sizeof(set->set));
    else
      FD_ZERO(&setread);

    if(t&eWrite)
      memcpy(&setwrite, &(set->set), sizeof(set->set));
    else
      FD_ZERO(&setwrite);

    if(t&eError)
      memcpy(&seterror, &(set->set), sizeof(set->set));
    else
      FD_ZERO(&seterror);

    return select(set->nfds, &setread, &setwrite, &seterror, (ms_timeout!=0)?(&times):NULL);

}
