#include "GLStart.hpp"
#include <cassert>
#include "Sapphire.hpp"

#ifdef OS_X

#include "RetinaCheck.h"
#include <SDL2/SDL_syswm.h>

#endif

using Sapphire::Galileo::GL::Operation;

namespace Sapphire{
namespace GL{

typedef struct ThreadKit * ThreadKitP;

ThreadKitP &GetSystemThreadkit(void){
    static ThreadKitP s;
    return s;
}

void EngineFlipScreenDelay(){

    while(AtomicGet(GetSystemThreadkit()->RenderFrame) + 32 < AtomicGet(GetSystemThreadkit()->EngineFrame)){
    }



}

concurrent_queue<Sapphire::Galileo::GL::Operation *> *RenderQueue(){
    return &(GetSystemThreadkit()->DrawQueue);
}

atomic32_t *GetRenderFrame(){
    return GetSystemThreadkit()->RenderFrame;
}

atomic32_t *GetEngineFrame(){
    return GetSystemThreadkit()->EngineFrame;
}

inline void SetSDL_GL_Attributes(void){
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

SDL_GLContext CreateForWindow(Window *aFor, Version aGLVersion){

    SetSDL_GL_Attributes();

    return SDL_GL_CreateContext(aFor->screen);

}

namespace MainThread{

    Window *CreateWindow(unsigned aWidth,unsigned aHeight,Version aGLVersion){
        Window *rWindow = new Window();

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, aGLVersion.major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, aGLVersion.minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);

        rWindow->screen = SDL_CreateWindow("Sapphire Heart",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            aWidth, aHeight, SDL2_VIDEO_FLAGS);

        rWindow->context = CreateForWindow(rWindow, aGLVersion);

        if(!rWindow->context)
            fprintf(stderr, BRACKNAME " Error: %s\n", SDL_GetError());

        printf(BRACKNAME " Info: Using OpenGL: %s\n", glGetString(GL_VERSION));

        assert(rWindow->context);

        return rWindow;
    }

}

namespace RenderThread{

    void *RenderThread(void *aKitv){
        ThreadKit *lKit = (ThreadKit *)aKitv;

        ClaimWindow(lKit->mWindow);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        //glEnable(GL_TEXTURE_2D);
        {
            SDL_SysWMinfo info;
            SDL_GetWindowWMInfo(lKit->mWindow->screen, &info);
            const char *subsystem = "?";
            switch(info.subsystem) {
                case SDL_SYSWM_UNKNOWN:   subsystem = "Unkown";  break;
                case SDL_SYSWM_WINDOWS:   subsystem = "Windows";  break;
                case SDL_SYSWM_X11:       subsystem = "X11";        break;
                case SDL_SYSWM_DIRECTFB:  subsystem = "DirectFB";               break;
                case SDL_SYSWM_COCOA:     subsystem = "Cocoa";             break;
                case SDL_SYSWM_UIKIT:     subsystem = "UIKit";                  break;
                case SDL_SYSWM_WAYLAND:   subsystem = "Wayland";                break;
                case SDL_SYSWM_MIR:       subsystem = "Mir";                    break;
            }
                printf(BRACKNAME " Info: Running on subsystem %s.\n", subsystem);

        }


        #ifdef OS_X

        {
            SDL_SysWMinfo info;
            SDL_GetWindowWMInfo(lKit->mWindow->screen, &info);
            if(info.subsystem==SDL_SYSWM_COCOA){
                float _f = TS_GetScalingFactor(info.info.cocoa.window);
                float _m = 1.0f;
                glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &_m);

                printf(BRACKNAME " Info: Running on Cocoa. Scaling factor of main window is %f. Maximum OpenGL raster scaling reported is %f.\n", _f, _m);



                glLineWidth(fmin(_f, _m));
            }
        }
        #endif


        SDL_GL_SetSwapInterval(0);

        Operation *Op = nullptr;

        //glUseProgram(3);

        while(AtomicGet(lKit->ShouldDie)==0){
            if(lKit->DrawQueue.try_pop(Op)==false){
                SDL_Delay(10);
                continue;
            }

            assert(Op);

            Op->Draw();
            if(!Op->IsPersistent()){
                delete Op;
                Op=nullptr;
            }

        }


        return nullptr;
    }

    void StartThread(Window *aWindow){
        ThreadKit *lKit = new ThreadKit();

        GetSystemThreadkit() = lKit;

        ClaimWindow(aWindow);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        glEnable(GL_SCISSOR_TEST);

       // glEnable(GL_TEXTURE_2D);

        lKit->mWindow = new Window();
        lKit->mWindow->screen = aWindow->screen;
        lKit->mWindow->context= CreateContextFor(aWindow, {3, 3});
        ClaimWindow(aWindow);

        lKit->EngineFrame = CreateAtomic(0);
        lKit->RenderFrame = CreateAtomic(0);
        lKit->ShouldDie   = CreateAtomic(0);
        lKit->DidDie      = CreateAtomic(0);

        lKit->Thread =CreateThread(RenderThread, lKit);
    }

    void StopThread(){
        ThreadKit *lKit = GetSystemThreadkit();
        AtomicSet(lKit->ShouldDie, 1);

        //lKit->DrawQueue.push(new DummyOperation<1> ());

        WaitThread(lKit->Thread);

        delete lKit->mWindow;
        delete lKit;

    }

    // Makes the given window available to the current thread. This is necessary
    // in order to create a context that is shared with the on associated with the
    // window.
    void ClaimWindow(Window *aClaim){
        SDL_GL_MakeCurrent(aClaim->screen, aClaim->context);
        printf(BRACKNAME " Info: Made\t%p\twindow, \t%p\tcontext\n", aClaim->screen, aClaim->context);

    }

    SDL_GLContext CreateContextFor(Window *aFor, Version aGLVersion){

        SDL_GLContext lNewContext;
        SDL_GLContext lOldContext = SDL_GL_GetCurrentContext();
        SDL_Window *lOldWindow =SDL_GL_GetCurrentWindow();
        SetSDL_GL_Attributes();
        ClaimWindow(aFor);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

        lNewContext = SDL_GL_CreateContext(aFor->screen);

        SDL_GL_MakeCurrent(lOldWindow, lOldContext);

        return lNewContext;

    }

}

}
}
