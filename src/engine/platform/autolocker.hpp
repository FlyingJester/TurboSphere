#pragma once

//! @file
//! @brief Simple RAII Wrapper intended for use with
//! @link Kashyyyk::LockingReciever @endlink objects, although compatible with
//! std::mutex objects or any class wiht a lock and unlock methodl.
//! @author    FlyingJester
//! @date      2014
//! @copyright GNU Public License 2.0

namespace Kashyyyk{

//!
//! @brief A simple RAII wrapper for classes with lock and unlock methods.
//!
//! This is primarily intended for Kashyyyk::LockingReciever objects, although
//! it will work with any class that has a lock and unlock method. Notably, it
//! is directly compatible with a std::mutex.
template <class T>
class AutoLocker{
protected:

    //! @cond

      T t;
      bool know;

    //! @endcond

public:

    //!
    //! @brief Calls lock. unlock will be called when the AutoLocker is
    //! destroyed.
    //!
    //! @param a object to wrap.
    AutoLocker(T a)
      : t(a)
      , know(true){
      t->lock();
    }


    ~AutoLocker(){
      if(know)
        t->unlock();
    }

    //!
    //! @brief Forget the wrapped object.
    //!
    //! It will not be unlocked when the AutoLocker is destroyed.
    void forget(){
      t->unlock();
      know = false;
    }

    //!
    //! @brief Change the object pointed to.
    //!
    //! This will not unlock the previously wrapped object (if one exists).
    //! @param a new object to wrap.
    void tell(T a){
      know = true;
      t = a;
    }

    };

}
