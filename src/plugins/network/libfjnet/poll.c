#include "poll.h"
#include "socket.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/poll.h>
#include <unistd.h>

#ifdef HAS_STRINGS
#include <strings.h>
#endif

#include <pthread.h>


#define LIBFJNET_INTERNAL
#include "socket_definition.h"

/*  Note that this backend is intended for Unix like systems that don't care if
    an FD is a socket or a file or a pipe.
*/


struct SocketSet{
    volatile struct pollfd *pollfds;
    int num_fds;
    int Pipe[2];
};

pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

int  IsPartOfSet_Internal(int fd, struct SocketSet *socket_set);

struct SocketSet *GenerateSocketSet(struct WSocket **sockets, unsigned num_sockets){
    int i = 0;
    struct SocketSet *socket_set = malloc(sizeof(struct SocketSet));

    socket_set->num_fds = num_sockets + 1;
    socket_set->pollfds = calloc(sizeof(struct pollfd), socket_set->num_fds);

    pipe(socket_set->Pipe);

    socket_set->pollfds[0].fd = socket_set->Pipe[0];
    socket_set->pollfds[0].events = POLLIN;

    for(; i<num_sockets; i++){
        socket_set->pollfds[i+1].fd = sockets[i]->sock;
        socket_set->pollfds[i+1].events = POLLIN;
    }

    return socket_set;

}

void FreeSocketSet(struct SocketSet *socket_set){
    free((void *)socket_set->pollfds);
    close(socket_set->Pipe[0]);
    close(socket_set->Pipe[1]);
    free(socket_set);
}


void PokeSet(struct SocketSet *socket_set){

    char r = '\a';
    write(socket_set->Pipe[1], &r, 1);
    read(socket_set->Pipe[0], &r, 1);

}


void AddToSet(struct WSocket *socket, struct SocketSet *socket_set){

    pthread_mutex_lock(&mx);

    {

        int found = 0, n = -1;

        n = IsPartOfSet_Internal(0, socket_set);

        if(n!=-1){
            found = 1;
            socket_set->pollfds[n].fd = socket->sock;
            socket_set->pollfds[n].events = POLLIN;
        }

        if(found)
            goto wake;

    }
    {
        struct pollfd *fds = NULL;

        void *old_ptr = (void *)socket_set->pollfds;

        fds = calloc(sizeof(struct pollfd), socket_set->num_fds+1);
        memcpy(fds, old_ptr, sizeof(struct pollfd)*socket_set->num_fds);

        fds[socket_set->num_fds].fd = socket->sock;
        fds[socket_set->num_fds].events = POLLIN;

        #ifdef __GNUC__
        while(!__sync_bool_compare_and_swap(&(socket_set->pollfds), socket_set->pollfds, fds)){}

        __sync_add_and_fetch(&socket_set->num_fds, 1);

        #else

            #error No Atomics?

        #endif

        free(old_ptr);
    }

wake:
    pthread_mutex_unlock(&mx);

    PokeSet(socket_set);

}

void RemoveFromSet(struct WSocket *socket, struct SocketSet *socket_set){
    int i = 0;
    pthread_mutex_lock(&mx);

    for(; i<socket_set->num_fds; i++){
        if(socket_set->pollfds[i].fd == socket->sock){
            socket_set->pollfds[i].fd = 0;
            break;
        }
    }

    pthread_mutex_unlock(&mx);

    PokeSet(socket_set);

}
void RemoveFromSetAndClose(struct WSocket *socket, struct SocketSet *socket_set){
    RemoveFromSet(socket, socket_set);

    Disconnect_Socket(socket);
}


int  IsPartOfSet_Internal(int socket, struct SocketSet *socket_set){
    int i = 0;
    for(; i<socket_set->num_fds; i++){
        if(socket_set->pollfds[i].fd == socket){
            return i;
            break;
        }
    }
    return -1;
}

int  IsPartOfSet(struct WSocket *socket, struct SocketSet *socket_set){
    int found = 0;
    pthread_mutex_lock(&mx);

    found = (IsPartOfSet_Internal(socket->sock, socket_set)!=-1);

    pthread_mutex_unlock(&mx);

    return found;

}

int  PollSet(enum WSockType t, struct SocketSet *socket_set, unsigned ms_timeout){

    return poll((struct pollfd *)socket_set->pollfds, socket_set->num_fds, ms_timeout);

}
