#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

struct RawMonitor {
    pthread_mutex_t mutex;
    pthread_cond_t  cv;
};

struct RawMonitor *Create_RawMonitor(){
    int err = 0;
    struct RawMonitor *monitor = malloc(sizeof(struct RawMonitor));
    pthread_condattr_t cv_attr;
    pthread_mutexattr_t mx_attr;

    err = pthread_condattr_init(&cv_attr);
    assert(err==0);
    err = pthread_mutexattr_init(&mx_attr);
    assert(err==0);

    err = pthread_cond_init(&(monitor->cv), &cv_attr);
    assert(err==0);

    err = pthread_mutex_init(&(monitor->mutex), &mx_attr);
    assert(err==0);

    err = pthread_condattr_destroy(&cv_attr);
    assert(err==0);
    err = pthread_mutexattr_destroy(&mx_attr);
    assert(err==0);

    return monitor;

}

void Destroy_RawMonitor(struct RawMonitor *monitor){
    int err;

    err = pthread_cond_destroy(&(monitor->cv));
    assert(err==0);

    err = pthread_mutex_destroy(&(monitor->mutex));
    assert(err==0);

    free(monitor);
}

void Lock_RawMonitor(struct RawMonitor *monitor){
    pthread_mutex_lock(&(monitor->mutex));
}

void Unlock_RawMonitor(struct RawMonitor *monitor){
    pthread_mutex_unlock(&(monitor->mutex));
}

void Wait_RawMonitor(struct RawMonitor *monitor){
    int err = pthread_cond_wait(&(monitor->cv), &(monitor->mutex));
    assert(err==0);
}

void Notify_RawMonitor(struct RawMonitor *monitor){
    int err = pthread_cond_signal(&(monitor->cv));
    assert(err==0);
}

void NotifyAll_RawMonitor(struct RawMonitor *monitor){
    int err = pthread_cond_broadcast(&(monitor->cv));
    assert(err==0);
}
