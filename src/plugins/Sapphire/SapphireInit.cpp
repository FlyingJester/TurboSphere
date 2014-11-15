#include "SapphireInit.hpp"

#include "GLStart.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "SaveImage.hpp"

#include <screen.h>

namespace Sapphire{

/////
// The SDL event filter that makes Sapphire well behaved.
//

int TS_Filter(void * _unused, SDL_Event *event){
if(event->type==SDL_QUIT){
        exit(0);
    }

    if(event->type==SDL_KEYDOWN){
        //printf("[SDL_GL] Info: Keydown Event.\n");
        int key = event->key.keysym.sym;
        if(!key) {
            return 1;
        }
    }
    return 1;
}

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

    printf(BRACKNAME " Info: SDL2 Video is initialized.\n");
    IMG_Init(IMG_FLAGS);

    Save::InitSurfaceSave();

    //TODO: Make this more lenient, ask for 4.4 and then lower versions if that fails
    GL::Version lVersion = {3, 3};

    GL::Window *lWindow = GL::MainThread::CreateWindow(GetScreenWidth(), GetScreenHeight(), lVersion);

    SDL_ShowCursor(0);

    SDL_SetEventFilter(TS_Filter, NULL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);
    glPointSize(2.0f);

    GL::RenderThread::StartThread(lWindow);

}

}
