#pragma once

typedef void TS_Mutex;

/////
// These are internal functions. It is not recommended to use these unless the C++ interface is unavailable.
//  A Mutex or semaphore backend must only define these functions.
extern "C" {
    TS_Mutex *TS_CreateMutex();
    void S_DestroyMutex(TS_Mutex *toDestroy);
    void TS_LockMutex(TS_Mutex *toLock);
    void TS_UnlockMutex(TS_Mutex *toUnlock);
}

namespace Sapphire {
    typedef TS_Mutex mutex_t;

    inline mutex_t *CreateMutex(){return TS_CreateMutex();}
    inline void DestroyMutex(mutex_t *a){TS_DestroyMutex(a);}
    inline void LockMutex(mutex_t *a){TS_LockMutex(a);}
    inline void UnlockMutex(mutex_t *a){TS_UnlockMutex(a);}
    
    struct AutoLocker{
        mutex_t *that;
        AutoLocker(mutex_t *t)
          : that(t){
            LockMutex(that);
        }
        ~AutoLocker(){
            UnlockMutex(that);
        }
    }

}
