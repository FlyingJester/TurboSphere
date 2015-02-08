
#include <libkern/OSAtomic.h>
#include <stdlib.h>

typedef void TS_Atomic32;

TS_Atomic32 *TS_CreateAtomic(int32_t aInitialValue){
    int32_t *rInt = malloc(4);
    *rInt = aInitialValue;
    return rInt;
}
int32_t TS_DestroyAtomic(TS_Atomic32 *aToDestroy){
    int32_t r = *((int32_t *)aToDestroy);
    free(aToDestroy);
    return r;
}

int32_t TS_AtomicGet(TS_Atomic32 * aToGet){
    return OSAtomicAdd32(0, aToGet);
}

void TS_AtomicSet(TS_Atomic32 * aToSet, int32_t aTo){
    // High level spinlock.
    while(!OSAtomicCompareAndSwap32(TS_AtomicGet(aToSet), aTo, aToSet)){}
}

int32_t TS_AtomicInc(TS_Atomic32 * aToInc){
  return OSAtomicIncrement32Barrier(aToInc);
}
int32_t TS_AtomicDec(TS_Atomic32 * aToDec){
  return OSAtomicDecrement32Barrier(aToDec);
}
int32_t TS_AtomicAdd(TS_Atomic32 * aTo, int32_t aToAdd){
    return OSAtomicAdd32Barrier(aToAdd, aTo);
}

int32_t TS_AtomicSub(TS_Atomic32 * aTo, int32_t aToSub){
    return OSAtomicAdd32Barrier(-aToSub, aTo);
}

int TS_AtomicCAS(TS_Atomic32 * aToSwap, int32_t aIfEqualTo, int32_t aTo){
    return OSAtomicCompareAndSwap32(aIfEqualTo, aTo, aToSwap);
}

