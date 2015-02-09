#pragma once

extern "C" {

    Turbo::Monitor::Mutex *Create_RawMonitor();

    void Destroy_RawMonitor(Turbo::Monitor::Mutex *m);

    void Lock_RawMonitor(Turbo::Monitor::Mutex *m);
    void Unlock_RawMonitor(Turbo::Monitor::Mutex *m);
    void Wait_RawMonitor(Turbo::Monitor::Mutex *m);
    void Notify_RawMonitor(Turbo::Monitor::Mutex *m);
    void NotifyAll_RawMonitor(Turbo::Monitor::Mutex *m);

}
