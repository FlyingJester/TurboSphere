#include <pthread.h>

pthread_t *TS_CreateThread(void *(*aFunction)(void *), void *aArg){
    pthread_t *rThread = malloc(sizeof(pthread_t));
    pthread_create(rThread, NULL, aFunction, aArg);
    return rThread;
}

void *TS_WaitThread(pthread_t *aThread){
    void *r;
    pthread_join(aThread, &r);
    return r;
}
