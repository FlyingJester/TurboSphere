#pragma once
#include <memory>

namespace Turbo{

//!
//! @brief A simple non-reentrant monitor that does not expose its locking
//! primitives
//!
//! This is intended to be a much simpler alternative to the
//! std::condition_variable, and does NOT require the use of unique_lockers or
//! even knowledge of the underlying mutex or locking system.
//!
//! This was inspired by the Mozilla Monitor system. A more elegant monitor of
//! a more civilized age.
//!
class Monitor{
public:

    //! Opaque mutex and state of a Monitor
    struct Mutex;
    struct MutexHolder;

    //! @brief Construct a Monitor and a Mutex
    //!
    //! This should be used for the first Monitor in a group. After that, all
    //! subsequent Monitors should be created using the first monitors Mutex.
    Monitor();

    //! @brief Construct a Monitor using a previously created Mutex
    //!
    //! This should be used to create Monitors that share a Mutex.
    //! @param m Mutex to use
    //! @sa GetMutex
    Monitor(std::shared_ptr<struct Monitor::MutexHolder> &m);

    //! @brief Synonymous to Monitor(that.GetMutex);
    Monitor(const Monitor &that);
    //! @brief Synonymous to Monitor(that->GetMutex);
    Monitor(const Monitor *that);

    ~Monitor();

    //! @brief Retrieve the Mutex from this Monitor
    //!
    //! This is required to construct subsequent Monitors that share a Mutex.
    //! All Monitors that share a Mutex are a part of the same group, and
    //! respond to any other monitor calling Notify or NotifyAll.
    const std::shared_ptr<struct Monitor::MutexHolder> &GetMutex() const;

    //! @brief Locks the Monitor
    //!
    //! The Monitor MUST be locked before Wait is called.
    void Lock();

    //! @brief Unlocks the Monitor
    //!
    //! The Monitor MUST be unlocked before Notify or NotifyAll is called.
    void Unlock();

    //! @brief Wait until Notify is called
    //!
    //! The Monitor MUST be locked before Wait is called.
    void Wait();

    //! @brief Equivalent to Lock, Wait
    inline void LockWait(){Lock(); Wait();}

    //! @brief Equivalent to Lock, Wait, Unlock
    inline void UnboundWait(){Lock(); Wait(); Unlock();}

    //! @brief Notify another Monitor in the same group
    //! The Monitor MUST be unlocked before Notify is called.
    void Notify();
    //! @brief Notify all Monitors in the same group
    //! The Monitor MUST be unlocked before NotifyAll is called.
    void NotifyAll();

    //! Wrapper for use with Kashyyyk::AutoLocker
    inline void lock(){Lock();}
    //! Wrapper for use with Kashyyyk::AutoLocker
    inline void unlock(){Unlock();}

    struct Locker{
        Monitor *that;
        Locker(Monitor *m)
          : that(m){
            that->Lock();
        }
          
        ~Locker(){
            that->Unlock();
        }
    };
    
    struct Locker CreateLocker(){
        return Locker(this);
    }

private:
    std::shared_ptr<struct MutexHolder> guts;
};

}
