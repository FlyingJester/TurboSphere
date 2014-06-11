#ifndef SAPPHIRE_INIT_HEAD
#define SAPPHIRE_INIT_HEAD
#include "sapphire.h"
#include <stdint.h>
#include "../common/plugin.h"
#include <list>

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS

namespace Sapphire {

    void Init(uint32_t ID);


    Turbo::ScriptObjectList<Turbo::JSCallback> *GetFunctions(void);
    Turbo::ScriptObjectList<Turbo::JSValue>    *GetVariables(void);

}


#endif
