#include <SDL2/SDL_atomic.h>

SDL_atomic_t *TS_CreateAtomic(int32_t aInitialValue){
    SDL_atomic_t* rAtom = malloc(sizeof(SDL_atomic_t));
    SDL_AtomicSet(rAtom, aInitialValue);
}

int32_t TS_DestroyAtomic(SDL_atomic_t *aToDestroy){
    int32_t r = SDL_AtomicGet(aToDestroy);
    free(aToDestroy);
    return r;
}

void TS_AtomicSet(SDL_atomic_t * aToSet, int32_t aTo){
    SDL_AtomicSet(aToSet, aTo);
}

int32_t TS_AtomicGet(SDL_atomic_t * aToGet){
    return SDL_AtomicGet(aToGet);
}

int32_t TS_AtomicInc(SDL_atomic_t * aToInc){
    return 1+SDL_AtomicAdd(aToInc, 1);
}

int32_t TS_AtomicDec(SDL_atomic_t * aToDec){
    return -1+SDL_AtomicAdd(aToDec, -1);
}
int32_t TS_AtomicAdd(SDL_atomic_t * aTo, int32_t aToAdd){
    return aToAdd+SDL_AtomicAdd(aToInc, aToAdd);
}
int32_t TS_AtomicSub(SDL_atomic_t * aTo, int32_t aToSub){
    return -aToAdd+SDL_AtomicAdd(aToInc, -aToAdd);
}
int TS_AtomicCAS(SDL_atomic_t * aToSwap, int32_t aIfEqualTo, int32_t aTo){
    return SDL_AtomicCAS(aToSwap,aIfEqualTo,aTo);
}
