#pragma once

typedef void TS_Mutex;

/////
// These are internal functions. It is not recommended to use these unless the C++ interface is unavailable.
//  A Mutex or semaphore backend must only define these functions.
extern "C" {
    TS_Mutex *TS_CreateMutex();
    TS_DestroyMutex(TS_Mutex *toDestroy);
    TS_LockMutex(TS_Mutex *toLock);
    TS_UnlockMutex(TS_Mutex *toUnlock);
}

namespace Sapphire {
    typedef TS_Mutex mutex_t;

    inline mutex_t *CreateMutex(){return TS_CreateMutex();}
    inline void DestroyMutex(mutex_t a){TS_DestroyMutex(a);}
    inline void LockMutex(mutex_t a){TS_LockMutex(a);}
    inline void UnlockMutex(mutex_t a){TS_UnlockMutex(a);}

}
