#ifndef SAPPHIRE_THREAD_EXTRA_HEAD
#define SAPPHIRE_THREAD_EXTRA_HEAD

/////
// Wrapper around pthreads, Win32 threads, or SDL2 threads.
//
// The decision is to choose:
//   native implementations first
//   standards-compliant implementation second (C++11 or Posix)
//   SDL2 or Intel TBB (if applicable) if neither of those are known/available
//
//
// The chosen thread type is transparent.
//
// You can call HaveThreads() to see if we found a threading library.
//


namespace Thread {

    void ThreadExtraInit(void);
    void ThreadExtraIClose(void);

    typedef void *mutex_t;
    typedef void *threaddata_t;

    enum MutexError {NoError = 0, Fatal = -1, Timeout = 1};
    enum ThreadPriority {Low = 1, Medium, High, Realtime};

    MutexError LockMutex(mutex_t);
    MutexError TryLockMutex(mutex_t);
    MutexError UnlockMutex(mutex_t);

    mutex_t CreateMutex( void);
    void    DestroyMutex(mutex_t);

    typedef void * (*ThreadFunction)(threaddata_t);

    typedef void *thread_t;

    thread_t CreateThread(ThreadFunction callback, const char *name, threaddata_t userdata);


    const char *GetThreadName(thread_t); //May return NULL even on a valid thread.

    void WaitThread(thread_t);

    //Called from the current thread.
    void SetThreadPriority(ThreadPriority); //This is not guaranteed to work on all systems.
    int GetThreadID(thread_t);


}

namespace Atomic{

    typedef void *atomic_t;

    atomic_t CreateAtomic(void);
    void DestroyAtomic(atomic_t);

    int AtomicGet(atomic_t);
    //Returns the previous value
    int AtomicSet(atomic_t, int);
    void AtomicInc(atomic_t);
    void AtomicDec(atomic_t);

}
#endif
