#pragma once



#include <stdlib.h>

typedef void TS_Atomic32;

/////
// These are internal functions. It is not recommended to use these unless the C++ interface is unavailable.
//  An atomic or thread backend must only define these functions.

#ifdef __cplusplus
extern "C" {
#endif

    TS_Atomic32 *TS_CreateAtomic(int32_t aInitialValue);
    int32_t TS_DestroyAtomic(TS_Atomic32 *aToDestroy); //Returns the last value.

    void TS_AtomicSet(TS_Atomic32 * aToSet, int32_t aTo);
    int32_t TS_AtomicGet(TS_Atomic32 * aToGet);
    int32_t TS_AtomicInc(TS_Atomic32 * aToInc); //Returns the new value.
    int32_t TS_AtomicDec(TS_Atomic32 * aToDec);
    int32_t TS_AtomicAdd(TS_Atomic32 * aTo, int32_t aToAdd);
    int32_t TS_AtomicSub(TS_Atomic32 * aTo, int32_t aToSub);
    int     TS_AtomicCAS(TS_Atomic32 * aToSwap, int32_t aIfEqualTo, int32_t aTo);

#ifdef __cplusplus
}


namespace Sapphire {

    typedef TS_Atomic32 atomic32_t;

    inline atomic32_t *CreateAtomic(int32_t aInitialValue){
        return TS_CreateAtomic(aInitialValue);
    }

    inline int32_t DestroyAtomic(atomic32_t *aToDestroy){ //Returns the last value.
        return TS_DestroyAtomic(aToDestroy); //Returns the last value.
    }

    inline void AtomicSet(atomic32_t * aToSet, int32_t aTo){
        TS_AtomicSet(aToSet, aTo);
    }

    inline int32_t AtomicGet(atomic32_t * aToGet){
        return TS_AtomicGet(aToGet);
    }

    inline int32_t AtomicInc(atomic32_t * aToInc){
        return TS_AtomicInc(aToInc);
    }

    inline int32_t AtomicDec(atomic32_t * aToDec){
        return TS_AtomicDec(aToDec);

    }

    inline bool AtomicCAS(atomic32_t * aToSwap, int32_t aIfEqualTo, int32_t aTo){
      return (TS_AtomicCAS(aToSwap, aIfEqualTo, aTo)==1);
    }

}
#endif
