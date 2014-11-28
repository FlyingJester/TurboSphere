#include "platform.hpp"

namespace Turbo {
    std::vector<Thread *> Platform::long_threads;
    std::vector<Thread *> Platform::short_threads;

    void Platform::Init(int long_threads_i, int short_threads_i){

        static bool first = true;

        if(first){
            first = false;

            for(int i = 0; i<long_threads_i; i++)
              Platform::long_threads.push_back(new Thread(Thread::GetLongThreadPool()));

            for(int i = 0; i<short_threads_i; i++)
              Platform::short_threads.push_back(new Thread(Thread::GetShortThreadPool()));

        }

    }

    Platform::Platform(int global_threads_i, int local_threads_i)
      : group(Thread::CreateTaskGroup())
      , global_threads_num(global_threads_i){

            for(int i = 0; i<global_threads_i; i++){
               Platform::long_threads.push_back(new Thread(Thread::GetLongThreadPool()));
               Platform::short_threads.push_back(new Thread(Thread::GetShortThreadPool()));
            }

            for(int i = 0; i<local_threads_i; i++)
              local_threads.push_back(new Thread(group));
    }

    void  Platform::CallOnBackgroundThread(v8::Task* task, v8::Platform::ExpectedRuntime expected_runtime){
        Thread::TaskGroup *g = group;
        if(expected_runtime==kShortRunningTask){
            g = Thread::GetShortThreadPool();
        }
        else if (expected_runtime==kLongRunningTask){
            g = Thread::GetLongThreadPool();
        }
        Thread::AddTask(g, task);
    }

    void  Platform::CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task){
        Thread::AddShortRunningTask(task);
       // Thread::AddTask(group, task);
    }

}
