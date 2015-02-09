#include <stdlib.h>
#include <Windows.h>
#include <assert.h>


struct RawMonitor {
	CONDITION_VARIABLE cv;
	CRITICAL_SECTION cs;
};

struct RawMonitor *Create_RawMonitor(){
    int err = 0;
    struct RawMonitor *monitor = malloc(sizeof(struct RawMonitor));

	InitializeConditionVariable(&(monitor->cv));
	InitializeCriticalSection(&(monitor->cs));
	
    return monitor;

}

void Destroy_RawMonitor(struct RawMonitor *monitor){
	
    WakeAllConditionVariable(&(monitor->cv));
	DeleteCriticalSection(&(monitor->cs));

	assert(monitor);
	free(monitor);
}

void Lock_RawMonitor(struct RawMonitor *monitor){
	EnterCriticalSection(&(monitor->cs));
}

void Unlock_RawMonitor(struct RawMonitor *monitor){
	LeaveCriticalSection(&(monitor->cs));
}

void Wait_RawMonitor(struct RawMonitor *monitor){
	SleepConditionVariableCS(&(monitor->cv), &(monitor->cs), INFINITE);
}

void Notify_RawMonitor(struct RawMonitor *monitor){
    WakeConditionVariable(&(monitor->cv));
}

void NotifyAll_RawMonitor(struct RawMonitor *monitor){
    WakeAllConditionVariable(&(monitor->cv));
}
