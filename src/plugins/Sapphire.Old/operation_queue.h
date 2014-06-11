#ifndef SAPPHIRE_OPERATION_QUEUE_HEAD
#define SAPPHIRE_OPERATION_QUEUE_HEAD

#ifdef __cplusplus
#include "loadfunc/thread_extra.h"
#include "loadfunc/enthread.h"
#include "../common/concurrent_queue.h"
extern concurrent_queue<std::pair< void(*)(void*), void * > > OperationQueue;


extern "C" {
#endif
    /////
    // Wrapper for languages without C++ namespace or (strong) mangling support.

    void Sapphire_EnqueueFunction(void(*func)(void *), void *callback);
    void Sapphire_StarveQueue(void);

    void Sapphire_OperationQueueInit(void);
    void Sapphire_OperationQueueClose(void);

#ifdef __cplusplus
}
#endif

#endif
