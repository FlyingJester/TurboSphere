#include <cstdio>
#include "monitor.hpp"
#include "monitor_backend.h"

namespace Turbo {

struct Monitor::MutexHolder {
    struct Mutex *mutex;
    explicit MutexHolder(struct Mutex *m)
      : mutex(m){

    }

    ~MutexHolder(){
        Destroy_RawMonitor(mutex);
    }
private:
    MutexHolder(){}
};

Monitor::Monitor()
  : guts(new MutexHolder(Create_RawMonitor())){
    printf("Created a mutex as %p\n", static_cast<void *>(guts->mutex));
}

Monitor::Monitor(std::shared_ptr<struct Monitor::MutexHolder> &m)
  : guts(m){
    printf("Created a mutex as %p\n", static_cast<void *>(guts->mutex));
}

Monitor::~Monitor() {
}

std::shared_ptr<struct Monitor::MutexHolder> &Monitor::GetMutex(){
    return guts;
};


void Monitor::Lock(){
    Lock_RawMonitor(guts->mutex);
}


void Monitor::Unlock(){
    Unlock_RawMonitor(guts->mutex);
}


void Monitor::Wait(){
    Wait_RawMonitor(guts->mutex);
}


void Monitor::Notify(){
    Notify_RawMonitor(guts->mutex);
}

void Monitor::NotifyAll(){
    NotifyAll_RawMonitor(guts->mutex);
}

}
