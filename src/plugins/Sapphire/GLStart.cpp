#include "GLStart.hpp"

#include "Thread/Atomic.hpp"
#include "Thread/Thread.hpp"
#include <pluginsdk/concurrent_queue.h>
#include "Galileo/Shape.hpp"

using Sapphire::Galileo::GL::Operation;

namespace Sapphire{
namespace GL{

struct ThreadKit{ // A recursive structure that describes and controls a renderthread.
    Window *mWindow; // Includes the destined context, not the creating thread's context.
    atomic32_t *EngineFrame;
    atomic32_t *RenderFrame;
    atomic32_t *ShouldDie;
    atomic32_t *DidDie;
    TS_Thread  *Thread;

    concurrent_queue<Operation *> DrawQueue;
};

typedef struct ThreadKit * ThreadKitP;

ThreadKitP &GetSystemThreadkit(void){
    static ThreadKitP s;
    return s;
}


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

    void *RenderThread(void *aKitv){
        ThreadKit *lKit = (ThreadKit *)aKitv;

        ClaimWindow(lKit->mWindow);

        Operation *Op = nullptr;

        while(AtomicGet(lKit->ShouldDie)==0){
            if(lKit->DrawQueue.try_pop(Op)==false){
                SDL_Delay(10);
                continue;
            }

            Op->Draw();

        }


        return nullptr;
    }

    void StartThread(Window *aWindow){
        ThreadKit *lKit = new ThreadKit();

        GetSystemThreadkit() = lKit;

        lKit->mWindow = new Window();
        lKit->mWindow->screen = aWindow->screen;
        lKit->mWindow->context= CreateContextFor(aWindow, {4, 1});

        lKit->EngineFrame = CreateAtomic(0);
        lKit->RenderFrame = CreateAtomic(0);
        lKit->ShouldDie   = CreateAtomic(0);
        lKit->DidDie      = CreateAtomic(0);

        lKit->Thread =CreateThread(RenderThread, lKit);

    }

    void StopThread();

    // Makes the given window available to the current thread. This is necessary
    // in order to create a context that is shared with the on associated with the
    // window.
    void ClaimWindow(Window *aClaim){
        SDL_GL_MakeCurrent(aClaim->screen, aClaim->context);

    }

    SDL_GLContext CreateContextFor(Window *aFor, Version aGLVersion){

        SDL_GLContext lNewContext;
        SDL_GLContext lOldContext = SDL_GL_GetCurrentContext();
        SDL_Window *lOldWindow =SDL_GL_GetCurrentWindow();

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, aGLVersion.major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, aGLVersion.minor);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

        SetSDL_GL_Attributes();

        ClaimWindow(aFor);

        lNewContext = SDL_GL_CreateContext(aFor->screen);

        SDL_GL_MakeCurrent(lOldWindow, lOldContext);

        return lNewContext;

    }

}

}
}
