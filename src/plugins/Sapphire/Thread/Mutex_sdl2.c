#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>

SDL_mutex *TS_CreateMutex(){return SDL_CreateMutex();}
TS_DestroyMutex(SDL_mutex *toDestroy){SDL_DestroyMutex(toDestroy);}
TS_LockMutex(SDL_mutex *toLock){SDL_LockMutex(toLock);}
TS_UnlockMutex(SDL_mutex *toUnlock){SDL_UnlockMutex(toUnlock);}
