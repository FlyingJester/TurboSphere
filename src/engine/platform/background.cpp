#include "background.hpp"
#include "autolocker.hpp"
#include "libyyymonitor/monitor.hpp"
#include <TSPR/concurrent_queue.h>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <cassert>

#ifdef _WIN32

#include <Windows.h>
#define MILLISLEEP(X) SleepEx(X, TRUE)

#else

#include <unistd.h>
#define MILLISLEEP(X) usleep(1000 * X)

#endif

namespace Turbo{

typedef std::pair<Thread::Thread_Impl &, std::atomic<bool> &> ThreadFunctionArg;
static void ThreadFunction(ThreadFunctionArg);

class Thread::TaskGroup{
  public:
    concurrent_queue<v8::Task *> queue;
    Monitor monitor;

    TaskGroup(){}

};

struct Thread::Thread_Impl{
    concurrent_queue<v8::Task *> &queue;
    Monitor monitor;
    std::atomic<bool> live;
    std::atomic<bool> started;
    std::thread thread;

    Thread_Impl(concurrent_queue<v8::Task *> &q, Monitor &mon)
      : queue(q)
      , monitor(mon.GetMutex())
      , live(true)
      , started(false)
      , thread(ThreadFunction, ThreadFunctionArg(*this, started)) {
        while(!started){
            monitor.NotifyAll();
        }
    }

};

static void ThreadFunction(ThreadFunctionArg input){
    Thread::Thread_Impl &thimble = input.first;
    // Call monitor.NotifyAll() until stated equals true to ensure the thread has started.

    thimble.monitor.UnboundWait();
    input.second = true;

    v8::Task * task;

    while(thimble.live){

        if(thimble.queue.try_pop(task)){

            task->Run();

        }

        thimble.monitor.UnboundWait();
    }
}


Thread::TaskGroup *Thread::GetShortThreadPool(){
    static TaskGroup group;
    return &group;
}


Thread::TaskGroup *Thread::GetLongThreadPool(){
    static TaskGroup group;
    return &group;
}


void Thread::AddLongRunningTask(v8::Task *task){
    Thread::GetLongThreadPool()->queue.push(task);
    Thread::GetLongThreadPool()->monitor.Notify();
}


void Thread::AddShortRunningTask(v8::Task *task){
    Thread::GetShortThreadPool()->queue.push(task);
    Thread::GetShortThreadPool()->monitor.Notify();
}


Thread::Thread(TaskGroup *group)
  : guts(new Thread::Thread_Impl(group->queue, group->monitor)){

}


Thread::~Thread(){
    guts->live = false;
    guts->monitor.NotifyAll();
    guts->thread.join();
}


void Thread::AddTask(TaskGroup *pool, v8::Task *task){
    pool->queue.push(task);
    pool->monitor.Notify();
}


void Thread::PerformTask(TaskGroup *pool){
    while(true){
        v8::Task * task;
        if(!pool->queue.try_pop(task))
          break;

        task->Run();

    }
}


Thread::TaskGroup *Thread::CreateTaskGroup(){
    Thread::TaskGroup *group = new Thread::TaskGroup();
    assert(group);
    return group;
}


void Thread::DestroyTaskGroup(Thread::TaskGroup *a){
    delete a;
}
/*

void NetworkWatch::ThreadFunction(NetworkWatch *that){
    while(that->live){
#if NEEDS_FJNET_POLL_TIMEOUT
        if(PollSet(eRead, that->socket_set, 100))
#else
        if(PollSet(eRead, that->socket_set, 0))
#endif
            that->monitor.Notify();

    }
}


NetworkWatch::NetworkWatch(Thread::TaskGroup *group)
  : live(true)
  , monitor(group->monitor.GetMutex())
  , socket_set(GenerateSocketSet(nullptr, 0))
  , thread(NetworkWatch::ThreadFunction, this){

}


NetworkWatch::NetworkWatch(std::shared_ptr<struct Monitor::MutexHolder> &mutex)
  : live(true)
  , monitor(mutex)
  , socket_set(GenerateSocketSet(nullptr, 0))
  , thread(NetworkWatch::ThreadFunction, this){

}

NetworkWatch::~NetworkWatch(){
    live = false;
    monitor.NotifyAll();
    PokeSet(socket_set);
    thread.join();
    FreeSocketSet(socket_set);

    monitor.NotifyAll();
}


void NetworkWatch::AddSocket(WSocket *socket){
    assert(!IsPartOfSet(socket, socket_set));
    AddToSet(socket, socket_set);
    assert(IsPartOfSet(socket, socket_set));
}


void NetworkWatch::DelSocket(WSocket *socket){
    assert(IsPartOfSet(socket, socket_set));
    RemoveFromSet(socket, socket_set);
    assert(!IsPartOfSet(socket, socket_set));
    PokeSet(socket_set);
}

void Thread::AddWatchToTaskGroup(NetworkWatch *watch, TaskGroup *group){
    group->watch = watch;
}


void Thread::AddSocketToTaskGroup(WSocket *socket, TaskGroup *group){
    group->watch->AddSocket(socket);
}

void Thread::RemoveSocketFromTaskGroup(WSocket *socket, TaskGroup *group){
    group->watch->DelSocket(socket);
}
*/

}
