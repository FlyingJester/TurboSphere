#ifndef SAPPHIRE_ERROR_HEAD
#define SAPPHIRE_ERROR_HEAD

/////
// A quick little function to make error reporting look nicer.
//
// This function should only be called by the main thread.
//

#include <SDL2/SDL_messagebox.h>

#ifdef __cplusplus
namespace Sapphire {
    inline void ThrowInternalError(const char *err){
        SDL_ShowSimpleMessageBox(0|SDL_MESSAGEBOX_ERROR, "TurboSphere Internal Error", err, NULL);
    }
}
#else
inline void TS_ThrowInternalError(const char *err){
    SDL_ShowSimpleMessageBox(0|SDL_MESSAGEBOX_ERROR, "TurboSphere Internal Error", err, NULL);
}
#endif

#endif
