/*
namespace thread {
    bool HaveThreads(void);

    typedef void *mutex_t;
    typedef void *threaddata_t;

    enum MutexError {NoError = 0, Fatal = -1, Timeout = 1};
    enum ThreadPriority {Low = 1, Medium, High, Realtime};

    MutexError LockMutex(mutex_t);
    MutexError TryLockMutex(mutex_t);
    MutexError UnlockMutex(mutex_t);

    mutex_t CreateMutex( void);
    void    DestroyMutex(mutex_t);

    typedef int (*ThreadFunction)(threaddata_t);

    typedef void *thread_t;

    thread_t CreateThread(ThreadFunction callback, const char *name, threaddata_t userdata);


    const char *GetThreadName(thread_t); //May return NULL even on a valid thread.

    WaitThread(thread_t);

    //Called from the current thread.
    void SetThreadPriority(ThreadPriority); //This is not guaranteed to work on all systems.
    int GetThreadID();


}

namespace atomic{

    typedef int atomic_t;

    int AtomicGet(atomic_t);
    //Returns the previous value
    int AtomicSet(atomic_t, int);
    void AtomicInc(atomic_t);
    void AtomicDec(atomic_t);
}
*/

#include "thread_extra.h"



using namespace Atomic;
using namespace Thread;

/////
// Atomics
#if (defined _WIN32)
/////
// Platform specific.


#elif defined HAS_ATOMIC
/////
// Standard.

#include <atomic>

typedef std::atomic<int> internal_atm;

Atomic::atomic_t Atomic::CreateAtomic(void){
    return new internal_atm;
}

void Atomic::DestroyAtomic(Atomic::atomic_t atm){
    delete static_cast<internal_atm*>(atm);
}

int Atomic::AtomicSet(Atomic::atomic_t atm, int val){
    internal_atm *RTF = static_cast<internal_atm*>(atm);
    int R = RTF->load();

    (*RTF) = val;

    return R;
}

void Atomic::AtomicInc(Atomic::atomic_t atm){
    internal_atm *RTF = static_cast<internal_atm*>(atm);
    (*RTF)++;
}

void Atomic::AtomicDec(Atomic::atomic_t atm){
    internal_atm *RTF = static_cast<internal_atm*>(atm);
    (*RTF)--;
}

#else
/////
// We always have SDL2.


#endif

/*

namespace Thread {

    typedef void *mutex_t;
    typedef void *threaddata_t;

    enum MutexError {NoError = 0, Fatal = -1, Timeout = 1};
    enum ThreadPriority {Low = 1, Medium, High, Realtime};

    MutexError LockMutex(mutex_t);
    MutexError TryLockMutex(mutex_t);
    MutexError UnlockMutex(mutex_t);

    mutex_t CreateMutex( void);
    void    DestroyMutex(mutex_t);

    typedef int (*ThreadFunction)(threaddata_t);

    typedef void *thread_t;

    thread_t CreateThread(ThreadFunction callback, const char *name, threaddata_t userdata);


    const char *GetThreadName(thread_t); //May return NULL even on a valid thread.

    void WaitThread(thread_t);

    //Called from the current thread.
    void SetThreadPriority(ThreadPriority); //This is not guaranteed to work on all systems.
    int GetThreadID();


}
*/


/////
// Threads
#if (defined _WIN32)
/////
// Platform specific.

#elif defined HAS_PTHREAD
#include <pthread.h>
#include <cstdlib>
#include <cstdio>

void ThreadExtraInit(void){

}
    #warning The warning means it is working.

    //ThreadPriority {Low = 1, Medium, High, Realtime};

    thread_t Thread::CreateThread(ThreadFunction callback, const char *name, threaddata_t userdata){
        pthread_t *thread = new pthread_t;
        pthread_create(thread, NULL, callback, userdata);
        return thread;
    }


    const char *GetThreadName(thread_t){return NULL;} //May return NULL even on a valid thread.

    void WaitThread(thread_t th){
        int lol = 0;
        int *lolp = &lol;
        int err = pthread_join(*static_cast<pthread_t *>(th), (void **)(&lolp));

        if(err!=0)
            fprintf(stderr, "[Sapphire] Thread Warning: A thread has been joined with an abnormal return value.\n");
    }


    //Called from the current thread.
    void SetThreadPriority(ThreadPriority); //This is not guaranteed to work on all systems.
    int Thread::GetThreadID(thread_t th){
        return (*static_cast<pthread_t *>(th));
    }


#elif defined HAS_THREAD
/////
// Standard.



#include <thread>
#include <cstdio>

typedef std::thread internal_thread;

void ThreadExtraInit(void){

}

thread_t Thread::CreateThread(ThreadFunction fn, const char *name, threaddata_t data){
    internal_thread *thread = new internal_thread(fn, data);
    return thread;

}

const char *Thread::GetThreadName(thread_t){
    return NULL;
}

void Thread::WaitThread(thread_t th){
    if(static_cast<internal_thread *>(th)->joinable()){
        static_cast<internal_thread *>(th)->join();
    }
    else{
        fprintf(stderr, "[Sapphire] Thread Warning: a thread became unjoinable. This probably means something very un-thread-safe happened.\n");
        static_cast<internal_thread *>(th)->detach();
        delete static_cast<internal_thread *>(th);
    }
}

void Thread::SetThreadPriority(ThreadPriority){

}

int Thread::GetThreadID(thread_t th){
    return static_cast<int>(static_cast<internal_thread *>(th)->native_handle());
}


#else
/////
// We always have SDL2.


#endif

/////
// Mutices
#if ((defined _WIN32) || (defined ANDROID) || (defined _BEOS))
/////
// Platform specific.

#elif defined HAS_MUTEX
/////
// Standard.


#else
/////
// We always have SDL2.


#endif
