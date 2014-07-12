#include <SDL2/SDL_thread.h>

SDL_Thread *TS_CreateThread(void *(*aFunction)(void *), void *aArg){
    static int ThreadN = 0;
    ThreadN++;

    char ThreadName[] = {
    'T', 'u', 'r', 'b', 'o',
    'S', 'p', 'h', 'e', 'r', 'e',
    '\0','\0','\0','\0','\0','\0','\0','\0'
    };

    sprintf(ThreadName+11, "%i", ThreadN);

    return SDL_CreateThread(aFunction, ThreadN,aArg);

}

void *TS_WaitThread(SDL_Thread *aThread){
    void *r;
    SDL_WaitThread(aThread, &r);
    return r;
}
