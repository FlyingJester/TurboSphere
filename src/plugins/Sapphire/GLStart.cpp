#include "GLStart.hpp"

namespace Sapphire{
namespace GL{

inline void SetSDL_GL_Attributes(void){
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

SDL_GLContext CreateForWindow(Window *aFor, Version aGLVersion){

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, aGLVersion.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, aGLVersion.minor);

    SetSDL_GL_Attributes();

    return SDL_GL_CreateContext(aFor->screen);

}

namespace MainThread{

    Window *CreateWindow(unsigned aWidth,unsigned aHeight,Version aGLVersion){
        Window *rWindow = new Window();

        rWindow->screen = SDL_CreateWindow("Sapphire Heart",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            aWidth, aHeight, SDL2_VIDEO_FLAGS);

        rWindow->context = CreateForWindow(rWindow, aGLVersion);

        return rWindow;
    }

}

namespace RenderThread{

    // Makes the given window available to the current thread. This is necessary
    // in order to create a context that is shared with the on associated with the
    // window.
    void ClaimWindow(Window *aClaim){


    }

    SDL_GLContext CreateContextFor(Window *aFor);

}

}
}
