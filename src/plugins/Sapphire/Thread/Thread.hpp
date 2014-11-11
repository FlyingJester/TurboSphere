#pragma once

#include <cstdlib>

typedef void TS_ThreadFuncArg;
typedef void TS_ThreadFuncRet;
typedef TS_ThreadFuncRet *(*TS_ThreadFunction)(TS_ThreadFuncArg *);
typedef void TS_Thread;

/////
// These are internal functions. It is not recommended to use these unless the C++ interface is unavailable.
//  An atomic or thread backend must only define these functions.
extern "C" {

    enum ThreadPriority {VeryLow = 1, Low, LowMedium, Medium, MediumHigh, High, VeryHigh, HighMaximum, Maximum, Thr_Default = 0, Unkown = -1};

    TS_Thread *TS_CreateThread(TS_ThreadFunction aFunction, TS_ThreadFuncArg *aArg);
    TS_ThreadFuncRet *TS_WaitThread(TS_Thread *aThread);
    void TS_SetThreadPriority(TS_Thread *aThread, enum ThreadPriority aPriority);
    enum ThreadPriority TS_GetThreadPriority(TS_Thread *aThread);

    void TS_SetCurrentThreadPriority(enum ThreadPriority aPriority);
    enum ThreadPriority TS_GetCurrentThreadPriority();
}

namespace Sapphire {

    typedef TS_Thread thread_t;
    typedef TS_ThreadFunction threadfunc_t;
    typedef TS_ThreadFuncArg threadarg_t;
    typedef TS_ThreadFuncRet threadret_t;

    inline thread_t *CreateThread(threadfunc_t aFunc, threadarg_t *aArg){
        return TS_CreateThread(aFunc, aArg);
    }

    inline threadarg_t *WaitThread(thread_t *aWaitFor){
        return TS_WaitThread(aWaitFor);
    }

    inline void SetThreadPriority(thread_t *aThread, ThreadPriority aPriority){
        TS_SetThreadPriority(aThread, aPriority);
    }

    inline ThreadPriority GetThreadPriority(thread_t *aThread){
        return TS_GetThreadPriority(aThread);
    }

    inline void SetCurrentThreadPriority(ThreadPriority aPriority){
        TS_SetCurrentThreadPriority(aPriority);
    }

    inline ThreadPriority GetCurrentThreadPriority(){
        return TS_GetCurrentThreadPriority();
    }

}
