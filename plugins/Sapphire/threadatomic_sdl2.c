#ifdef HAS_ATOMIC
    #ifdef _MSC_VER
        #pragma warning Using SDL atomics, even though C++11 atomics are available.
    #else
        #warning Using SDL atomics, even though C++11 atomics are available.
    #endif
#endif

#include "surface_thread.h"
#include <SDL2/SDL_atomic.h>

#include <assert.h>

//Prep a ListItem
void CreateListItem(struct ListItem *_l){
    assert(_l);
    _l->next_atm    = NULL;
    _l->prev_atm    = NULL;
    _l->_ref        = malloc(sizeof(SDL_atomic_t));
    SDL_AtomicSet(_l->_ref, 0);
}

void DestroyListItem(struct ListItem *_l){
    assert(_l);
    free(_l->_ref);
}

// Atomically mod the reference variable.
void IncListItemRef(struct ListItem *_l){
    assert(_l);
    SDL_AtomicIncRef(_l->_ref);
}

void DecListItemRef(struct ListItem *_l){
    assert(_l);
    SDL_AtomicDecRef(_l->_ref);
}

// Atomically retrieve the reference count of the list item.
unsigned GetListItemRef(struct ListItem *_l){
    assert(_l);
    return SDL_AtomicGet(_l->_ref);
}

// Mark the list item as near death.
int InvListItemRef(struct ListItem *_l, int force){
    assert(_l);
    if(force){
        SDL_AtomicSet(_l->_ref, 0xFFFFFFFFu);
        return 0;
    }
    return !SDL_AtomicCAS(_l->_ref, 0, 0xFFFFFFFFu);
}

// Atomically set the raw pointers in the list item.
void SetListItemNext(struct ListItem *_l, struct ListItem *set_to){
    assert(_l);
    SDL_AtomicSetPtr(&(_l->next_atm), set_to);
}

void SetListItemPrev(struct ListItem *_l, struct ListItem *set_to){
    assert(_l);
    SDL_AtomicSetPtr(&(_l->prev_atm), set_to);
}

void *GetListItemNext(struct ListItem *_l){
    assert(_l);
    return SDL_AtomicGetPtr(&(_l->next_atm));
}

void *GetListItemPrev(struct ListItem *_l){
    assert(_l);
    return SDL_AtomicGetPtr(&(_l->prev_atm));
}

