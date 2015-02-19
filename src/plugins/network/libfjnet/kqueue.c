#include "poll.h"
#include "socket.h"
#define LIBFJNET_INTERNAL
#include "socket_definition.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

static struct timespec time_immediate;

struct SocketSet {
#ifndef NDEBUG
    int *sockets;
    unsigned num_sockets;
#endif
    int queue;
};

struct SocketSet *GenerateSocketSet(struct WSocket **sockets,
                                    unsigned num_sockets){

    int i = 0;
    struct SocketSet *socket_set = malloc(sizeof(struct SocketSet));
    struct kevent *events = malloc(sizeof(struct kevent)*(num_sockets+1));

#ifndef NDEBUG
    socket_set->sockets = malloc(sizeof(int)*num_sockets);
    socket_set->num_sockets = num_sockets;
#endif

    for (;i<num_sockets; i++){
        EV_SET(&(events[i+1]), sockets[i]->sock, EVFILT_READ, EV_ADD,
               0, 0, 0);

#ifndef NDEBUG
        socket_set->sockets[i] = sockets[i]->sock;
#endif

    }

    MS_INTO_TIMESPEC(time_immediate, 0);

    socket_set->queue = kqueue();
    kevent(socket_set->queue, events, num_sockets+1, NULL, 0, &time_immediate);

    free(events);

    return socket_set;
}


void FreeSocketSet(struct SocketSet *socket_set){

    free(socket_set);

}


void PokeSet(struct SocketSet *socket_set){
    struct kevent event;

    EV_SET(&event, 0xFF, EVFILT_TIMER, EV_ADD|EV_ONESHOT, 0, 0, 0);
    kevent(socket_set->queue, &event, 1, NULL, 0, &time_immediate);
}


void AddToSet(struct WSocket *socket, struct SocketSet *socket_set){
    struct kevent events[2];
#ifndef NDEBUG
    socket_set->sockets = realloc(socket_set->sockets,
                                  (socket_set->num_sockets+1)*sizeof(int));
    socket_set->sockets[socket_set->num_sockets] = socket->sock;
    socket_set->num_sockets++;
#endif

    EV_SET(&(events[0]), socket->sock, EVFILT_READ, EV_ADD, 0, 0, 0);
    EV_SET(&(events[1]), socket->sock, EVFILT_TIMER, EV_ADD|EV_ONESHOT, 0, 0, 0);

    kevent(socket_set->queue, events, 2, NULL, 0, &time_immediate);

}


void DebugRemoveFromSet(struct WSocket *socket, struct SocketSet *socket_set){
#ifndef NDEBUG
    int i = 0, found = 0;
    for(; i<socket_set->num_sockets; i++){
        if(socket_set->sockets[i]==socket->sock){
            found = 1;
            break;
        }
    }

    for(i++; i<socket_set->num_sockets; i++){
        socket_set->sockets[i-1] =socket_set->sockets[i];
    }

    socket_set->num_sockets-=found;
    socket_set->sockets = realloc(socket_set->sockets,
                                  socket_set->num_sockets*sizeof(int));
#endif
}

void RemoveFromSet(struct WSocket *socket, struct SocketSet *socket_set){
    struct kevent events[2];

    EV_SET(&(events[0]), socket->sock, EVFILT_READ, EV_DELETE, 0, 0, 0);
    EV_SET(&(events[1]), socket->sock, EVFILT_TIMER, EV_ADD|EV_ONESHOT, 0, 0, 0);

    kevent(socket_set->queue, events, 2, NULL, 0, &time_immediate);

    DebugRemoveFromSet(socket, socket_set);

}


void RemoveFromSetAndClose(struct WSocket *socket, struct SocketSet *socket_set){

    DebugRemoveFromSet(socket, socket_set);
    Disconnect_Socket(socket);

    PokeSet(socket_set);

}


int  IsPartOfSet(struct WSocket *socket, struct SocketSet *socket_set){
#ifndef NDEBUG
    int i = 0;
    for(; i<socket_set->num_sockets; i++){
        if(socket_set->sockets[i]==socket->sock)
          return 1;
    }
    return 0;
#else
    return 1;
#endif
}


int PollSet(enum WSockType t, struct SocketSet *socket_set, unsigned ms_timeout){

    struct timespec times;
    struct kevent event;
    int n = 0;

    assert(t==eRead);

    MS_INTO_TIMESPEC(times, ms_timeout);

    n = kevent(socket_set->queue, NULL, 0, &event, 1, (ms_timeout==0)?NULL:(&times));

    return n;

}
