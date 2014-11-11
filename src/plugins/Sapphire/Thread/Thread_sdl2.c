#include <SDL2/SDL_thread.h>
#include <assert.h>

SDL_Thread *TS_CreateThread(void *(*aFunction)(void *), void *aArg){
    static int ThreadN = 0;
    ThreadN++;

    assert(aFunction);

    char ThreadName[] = {
    'T', 'u', 'r', 'b', 'o',
    'S', 'p', 'h', 'e', 'r', 'e',
    '\0','\0','\0','\0','\0','\0','\0','\0'
    };

    sprintf(ThreadName+11, "%i", ThreadN);

    return SDL_CreateThread(aFunction, ThreadN,aArg);

}

void *TS_WaitThread(SDL_Thread *aThread){
    assert(aThread);
    void *r;
    SDL_WaitThread(aThread, &r);
    return r;
}

void TS_SetThreadPriority(SDL_Thread *aThread, int aPriority){
    assert(aThread);
    assert(aPriority>=0 && aPriority<10);

    //Unfortunately, there's not much else that we can do.

}

int TS_GetThreadPriority(SDL_Thread *aThread){
    assert(aThread);
    return -1;
}

void TSPR_Secret_null_destructor(void* value){}

void TS_SetCurrentThreadPriority(int aPriority){
    int p = SDL_THREAD_PRIORITY_LOW;
    if(aPriority > 2)
      p = SDL_THREAD_PRIORITY_NORMAL;
    if(aPriority > 6)
      p = SDL_THREAD_PRIORITY_HIGH;

    SDL_SetThreadPriority(p);
}

enum ThreadPriority TS_GetCurrentThreadPriority(){
    return -1;
};
