#include "operation_queue.h"

#include <cassert>
#include <cstdlib>

struct ThreadFuncArg{
    concurrent_queue<std::pair< void(*)(void*), void * > > cqueue;
    Atomic::atomic_t die_flag;
    Atomic::atomic_t dead_flag;
};

void GLThreadFunction(ThreadFuncArg *tfa);

concurrent_queue<std::pair< void(*)(void*), void * > > *cqueue = NULL;

void Sapphire_OperationQueueInit(void){

    ThreadFuncArg *ta = (ThreadFuncArg *)malloc(sizeof(ThreadFuncArg));

    ta->cqueue = concurrent_queue<std::pair< void(*)(void*), void * > >();
    cqueue = &(ta->cqueue);
    ta->die_flag = Atomic::CreateAtomic();
    ta->dead_flag= Atomic::CreateAtomic();

    Atomic::AtomicSet(ta->die_flag,  0);
    Atomic::AtomicSet(ta->dead_flag, 0);
    Thread::CreateThread((Thread::ThreadFunction)GLThreadFunction, "OpenGL_Thread", (Thread::threaddata_t)ta);

}

void Sapphire_OperationQueueClose(void){


/*
    while(!Atomic::AtomicGet(queue_died)){}

    Atomic::DestroyAtomic(queue_died);*/
}

void Sapphire_EnqueueFunction(void(*func)(void *), void *callback){
    cqueue->push(std::pair< void(*)(void*), void * >(func, callback));
}

void Sapphire_StarveQueue(void);

void GLThreadFunction(ThreadFuncArg *tfa){
    assert(tfa != NULL);

    std::pair<void (*)(void *), void *> operation;

    while(!Atomic::AtomicGet(tfa->die_flag)){
        if(tfa->cqueue.try_pop(operation))
            operation.first(operation.second);
    }

    Atomic::AtomicSet(tfa->dead_flag, 1);

    Atomic::DestroyAtomic(tfa->die_flag);

}
