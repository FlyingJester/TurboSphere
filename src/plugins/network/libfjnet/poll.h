#pragma once
#include "status.h"

#ifndef NEEDS_FJNET_POLL_TIMEOUT
#define NEEDS_FJNET_POLL_TIMEOUT 1
#endif

#ifdef __cplusplus
extern "C" {
#endif
struct SocketSet;


struct SocketSet *GenerateSocketSet(struct WSocket **sockets, unsigned num_sockets);
void FreeSocketSet(struct SocketSet *set);

void AddToSet(struct WSocket *sockets, struct SocketSet *set);
void RemoveFromSet(struct WSocket *sockets, struct SocketSet *set);
void RemoveFromSetAndClose(struct WSocket *sockets, struct SocketSet *set);
int  IsPartOfSet(struct WSocket *sockets, struct SocketSet *set);

int  PollSet(enum WSockType t, struct SocketSet *set, unsigned ms_timeout);

void PokeSet(struct SocketSet *set);

#ifdef __cplusplus
}
#endif

