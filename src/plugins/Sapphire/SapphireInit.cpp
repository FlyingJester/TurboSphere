#include "SapphireInit.hpp"

#include "GLStart.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <screen.h>

namespace Sapphire{

/////
// The init process for Sapphire's threaded OpenGL renderer:
//
// Init SDL2
// Init SDL_image
// Create Window w/ GL context
// Create sharing, Render GL context
// Create Render thread
// Claim Window and Render GL context on Render thread.
//
// At that point, the render thread can start spinning on the render queue.
//
//

void Init(uint64_t ID){
    auto lInited = SDL_WasInit(SDL_INIT_EVERYTHING);
    if(lInited==0)
        SDL_Init(SDL_INIT_VIDEO);
    else if((lInited&SDL_INIT_VIDEO)!=SDL_INIT_VIDEO)
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    else
        fprintf(stderr, P_WARNING "SDL2 Video was already initialized. Another video plugin loaded?\n");

    IMG_Init(IMG_FLAGS);

    GL::Version lVersion = {4, 1};

    GL::Window *lWindow = GL::MainThread::CreateWindow(GetScreenWidth(), GetScreenHeight(), lVersion);

    GL::RenderThread::StartThread(lWindow);

    //SDL_GLContext lRenderThreadContext =  CreateContextFor(lWindow, lVersion);


}

}
