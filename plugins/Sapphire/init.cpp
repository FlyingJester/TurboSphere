#include "init.hpp"
#include "primitives.h"
#include "blendmodes.h"

#include "color.hpp"
#include "screen.hpp"
#include "image.hpp"
#include "surface.h"
#include "galileo/shape.hpp"
#include "galileo/group.hpp"

#include "glextra/gl_extra.h"
#include "flipscreen.hpp"

#include "error.h"

#include <SDL2/SDL.h>
#include <cassert>

#include <string>

#include "../../configmanager/opengame.h"

const char * const TSWindowTitle = "TurboSphere Game Engine";
std::string WindowTitle;

inline const char *TS_BlendModeNames(size_t i) {
    static const char *tnames[] = {
        "BLEND",
        "REPLACE",
        "RGB_ONLY",
        "ALPHA_ONLY",
        "ADD",
        "SUBTRACT",
        "MULTIPLY",
        "AVERAGE",
        "INVERT",
        "SQUARE"
    };
    assert(i<sizeof(tnames));

    return tnames[i];
}

template <class T>
inline void CheckSOL(Turbo::ScriptObjectList<T> *objectlist) {
    if(!(objectlist->GetA()&&objectlist->GetNames())) {

        int msg_box_err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                          "Sapphire Script Object List Error",
                          "An internal error has occured. The script function list was not initialized.",
                          NULL);

        assert(!msg_box_err);
        abort();
    }
}

Turbo::ScriptObjectList<Turbo::JSCallback> *Functions = NULL;
Turbo::ScriptObjectList<Turbo::JSValue> *Variables = NULL;

const unsigned int num_funcs = 6;
const unsigned int num_vars  = (
                                   NUM_BLENDMODES+
                                   0
                               );

void Sapphire::Init(uint32_t ID) {

    const TS_Config *tsconf = GetConfig();

    WindowTitle = TSWindowTitle;

    if(tsconf->gamename){
        WindowTitle += " - ";
        WindowTitle += tsconf->gamename;
    }

    SDL_GLContext glcontext;

    SDL_Window *screen = NULL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    screen = SDL_CreateWindow(WindowTitle.c_str(),
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              tsconf->screenwidth, tsconf->screenheight, SDL2_VIDEO_FLAGS);

    if(!screen)
        Sapphire::ThrowInternalError("Could not create a window.");

    glcontext = SDL_GL_CreateContext(screen);

    if(!glcontext)
        Sapphire::ThrowInternalError("Could not create an OpenGL 3.2 context.");

    if(!glcontext&&screen)
        exit(EXIT_FAILURE);

    Sapphire::SetWindowToFlip(screen);

    LoadGLFunctions();
    printf("[" PLUGINNAME "] Info: Using OpenGL version %s\n", glGetString(GL_VERSION));

    { //These are only for some debugging.
        //glClearColor(0, 0, 0, 255);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
    }
    /*{
        void ** ret = malloc(sizeof(void *)<<1);

        ret[0] = screen;
        ret[1] = glcontext;

        return ret;
    }*/


    InitDrawQueue();
    InitColor(ID);
    InitPrimitives(ID);

    SurfaceInit(ID);

    Sapphire::InitImage(ID, iso);
    Sapphire::Galileo::InitShape(ID, iso);
    Sapphire::Galileo::InitGroup(ID, iso);

    Functions = new Turbo::ScriptObjectList<Turbo::JSCallback>(num_funcs);
    Variables = new Turbo::ScriptObjectList<Turbo::JSValue>(num_vars);

    Functions->Set((Turbo::JSCallback)CreateColor, "Color", 0u);
    Functions->Set((Turbo::JSCallback)Sapphire::Galileo::Script::CreateShape, "Shape", 1u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::CreateSurface, "Surface", 2u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::CreateImage, "Image", 3u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::GetScreenWidth, "GetScreenWidth", 4u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::GetScreenHeight, "GetScreenHeight", 5u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::FlipScreen, "FlipScreen", 6u);

//Set blendmodes.
    for(int i = 0; i<(int)num_vars; i++)
        Variables->Set(v8::Integer::New(iso, i), TS_BlendModeNames(i), i);

}

Turbo::ScriptObjectList<Turbo::JSCallback> *Sapphire::GetFunctions(void) {
    CheckSOL(Functions);
    return Functions;
}

Turbo::ScriptObjectList<Turbo::JSValue>    *Sapphire::GetVariables(void) {
    CheckSOL(Variables);
    return Variables;
}
