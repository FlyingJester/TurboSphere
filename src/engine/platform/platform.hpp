#pragma once
#include <v8.h>
#include <v8-platform.h>
#include <memory>
#include <vector>
#include "background.hpp"

#include "time.hpp"

namespace Turbo {

class Platform : public v8::Platform {

    Thread::TaskGroup *group;

    std::vector<Thread *> local_threads;
    static std::vector<Thread *> long_threads;
    static std::vector<Thread *> short_threads;

    int global_threads_num;

public:

    // The first constructor call also calls this.
    // This functon is only meaningfulyl called once.
    static void Init(int long_threads = 2, int short_threads = 2);

    Platform(int global_threads = 1, int local_threads = 4);

    void CallOnBackgroundThread(v8::Task* task,
                                      v8::Platform::ExpectedRuntime expected_runtime) override;

    void CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task) override;

    ~Platform() override {Thread::DestroyTaskGroup(group);}
    double MonotonicallyIncreasingTime() override {return TS_GetSeconds();}

};

}
