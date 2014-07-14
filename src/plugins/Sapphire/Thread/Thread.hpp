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
    TS_Thread *TS_CreateThread(TS_ThreadFunction aFunction, TS_ThreadFuncArg *aArg);
    TS_ThreadFuncRet *TS_WaitThread(TS_Thread *aThread);
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

}
