#ifndef SAPPHIRE_THREADER_HEAD
#define SAPPHIRE_THREADER_HEAD

#include "loadfunc/thread_extra.h"

/////
// Threader Header
//
// Contains base class for singleton switchable multi/single threader classes to derive from
//
//
//

template<class T>
class SwitchableThreaderBase{
    public:
    SwitchableThreaderBase();
    ~SwitchableThreaderBase();

    virtual void EnthreadCallback(T) = 0;
    virtual void DethreadCallback(T) = 0;

    void Enthread(){
        EnthreadCallback(enthreaddata);
    }

    void Dethread(){
        DethreadCallback(dethreaddata);
    }

    T enthreaddata;
    T dethreaddata;
};

#endif
