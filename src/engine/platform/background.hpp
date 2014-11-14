#pragma once
#include <v8-platform.h>
#include <memory>

namespace Turbo {

class Thread{
public:

    class TaskGroup;

    Thread(TaskGroup *taskgroup);

    ~Thread();

    static TaskGroup *GetShortThreadPool();

    static TaskGroup *GetLongThreadPool();

    static void AddTask(TaskGroup *group, v8::Task *task);

    static void PerformTask(TaskGroup *group);

    static void AddLongRunningTask(v8::Task *task);
    static void AddShortRunningTask(v8::Task *task);

    static TaskGroup *CreateTaskGroup();

    static void DestroyTaskGroup(TaskGroup *task);
/*
    static void AddWatchToTaskGroup(NetworkWatch *watch, TaskGroup *group);
    static void AddSocketToTaskGroup(WSocket *socket, TaskGroup *group);
    static void RemoveSocketFromTaskGroup(WSocket *socket, TaskGroup *group);
*/

    struct Thread_Impl;

private:
    std::unique_ptr<Thread_Impl> guts;
};

}
