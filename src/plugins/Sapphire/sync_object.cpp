#include "sync_object.hpp"

#ifdef HAS_ATOMIC

#include <cinttypes>
#include <atomic>

typedef std::atomic_uint_fast32_t TS_Atomic;

struct Sapphire::_PSpriv{
    TS_Atomic atm;
};

void Sapphire::PollSynchro::Ready(int val){
    _p->atm = val;
}

bool Sapphire::PollSynchro::isDone(void){
    return _p->atm.load();
}

#else
#include <SDL2/SDL_atomic.h>
typedef SDL_atomic_t TS_Atomic;

struct Sapphire::_PSpriv{
    TS_Atomic *atm;
};

void Sapphire::PollSynchro::Ready(int val){
    SDL_AtomicSet(_p->atm, val);
}

bool Sapphire::PollSynchro::isDone(void){
    return SDL_AtomicGet(_p->atm);
}

Sapphire::Synchro::Synchro(){
    //Forces this translation unit to hold the vtable for Sapphire::Synchro.
    nn = NULL;
}

Sapphire::Synchro::~Synchro(){

}

Sapphire::PollSynchro::PollSynchro(){

}

Sapphire::PollSynchro::~PollSynchro(){

}

Sapphire::CallbackSynchro::CallbackSynchro(){

}

Sapphire::CallbackSynchro::~CallbackSynchro(){

}

#endif
