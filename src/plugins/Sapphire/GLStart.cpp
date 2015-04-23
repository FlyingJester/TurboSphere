#include "GLStart.hpp"
#include <array>
#include <cassert>
#include "Sapphire.hpp"

#ifdef OS_X

#include "RetinaCheck.h"
#include <SDL2/SDL_syswm.h>

#endif

// Begin swizzling of context types!

static const unsigned num_context_types = 3;
static const std::array<unsigned, num_context_types> context_types = {{
#ifdef OPENGL_4
    SDL_GL_CONTEXT_PROFILE_CORE,
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
    SDL_GL_CONTEXT_PROFILE_ES
#elif defined(OPENGL_2)
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
    SDL_GL_CONTEXT_PROFILE_CORE,
    SDL_GL_CONTEXT_PROFILE_ES
#elif defined(OPENGL_ES)
    SDL_GL_CONTEXT_PROFILE_ES,
    SDL_GL_CONTEXT_PROFILE_CORE,
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
#else
    SDL_GL_CONTEXT_PROFILE_CORE,
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
    SDL_GL_CONTEXT_PROFILE_ES
#endif
}};

static const std::array<const char *, num_context_types> context_names = {{
#ifdef OPENGL_4
    "core",
    "compatibility",
    "ES"
#elif defined(OPENGL_2)
    "compatibility",
    "core",
    "ES"
#elif defined(OPENGL_ES)
    "ES",
    "core",
    "compatibility"
#else
    "core",
    "compatibility",
    "ES"
#endif
}};

// End of swizzling of context types!

using Sapphire::Galileo::GL::Operation;

namespace Sapphire{
namespace GL{

typedef struct ThreadKit * ThreadKitP;

ThreadKitP &GetSystemThreadkit_internal(void){
    static ThreadKitP s;
    return s;
}
ThreadKitP GetSystemThreadkit(void){
    return GetSystemThreadkit_internal();
}

inline void SetSDL_GL_Attributes(void){
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

SDL_GLContext CreateForWindow(Window *aFor, const Version &aGLVersion){

    SetSDL_GL_Attributes();

    return SDL_GL_CreateContext(aFor->screen);

}

namespace MainThread{
    
    bool CreateWindowOpt(Window *window, const Version &gl_version, int profile){
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_version.major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_version.minor);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  profile);

        window->context = CreateForWindow(window, gl_version);
        return window->context;
    } 
    
    template<unsigned t>
    static bool UseContext_(Window *window, const Version &gl_version){
        if(CreateWindowOpt(window, gl_version, context_types[t])){
            printf(BRACKNAME " Info using %s OpenGL context\n", context_names[t]);
            return true;
        }
        return UseContext_<t+1>(window, gl_version);
    }
    
    template<>
    bool UseContext_<num_context_types>(Window *window, const Version &gl_version){ return false; }

    Window *CreateWindow(unsigned width, unsigned height, const Version &gl_version){
        Window *rWindow = new Window();
        rWindow->screen = SDL_CreateWindow("Sapphire Heart",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, SDL2_VIDEO_FLAGS);

        UseContext_<0>(rWindow, gl_version);
                
        if(!rWindow->context){
            fprintf(stderr, BRACKNAME " Error: %s\n", SDL_GetError());
        }
        else{
            printf(BRACKNAME " Info using OpenGL: %s\n", glGetString(GL_VERSION));
        }
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

        {
        #ifdef OS_X
            SDL_SysWMinfo info;
            SDL_GetWindowWMInfo(lKit->mWindow->screen, &info);
            if(info.subsystem==SDL_SYSWM_COCOA){
                float _f = TS_GetScalingFactor(info.info.cocoa.window);
                float _m = 1.0f;
                glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &_m);

                printf(BRACKNAME " Info: Running on Cocoa. Scaling factor of main window is %f. Maximum OpenGL raster scaling reported is %f.\n", _f, _m);

                glLineWidth(fmin(_f, _m));
            }
        #else
            SDL_SysWMinfo info;
            SDL_GetWindowWMInfo(lKit->mWindow->screen, &info);
            const char *subsystem = "?";
            switch(info.subsystem) {
                case SDL_SYSWM_WINRT:
                case SDL_SYSWM_UNKNOWN:   subsystem = "Unkown";         break;
                case SDL_SYSWM_WINDOWS:   subsystem = "Windows";        break;
                case SDL_SYSWM_X11:       subsystem = "X11";            break;
                case SDL_SYSWM_DIRECTFB:  subsystem = "DirectFB";       break;
                case SDL_SYSWM_COCOA:     subsystem = "Cocoa";          break;
                case SDL_SYSWM_UIKIT:     subsystem = "UIKit";          break;
                case SDL_SYSWM_WAYLAND:   subsystem = "Wayland";        break;
                case SDL_SYSWM_MIR:       subsystem = "Mir";            break;
            }
                printf(BRACKNAME " Info: Running on subsystem %s.\n", subsystem);
        #endif
        }

        SDL_GL_SetSwapInterval(0);

        Operation *Op = nullptr;

        while(AtomicGet(lKit->ShouldDie)==0){

            lKit->monitor.Lock();
            
            std::queue<Sapphire::Galileo::GL::Operation *> &queue = lKit->Queues[lKit->render_from];

            if(queue.empty()){
                AdvanceRenderQueue<ThreadKit &>(*lKit);

                lKit->monitor.Wait();
                lKit->monitor.Unlock();
                continue;
            }

            Op = queue.front();
            assert(Op);
            queue.pop();

            if(Op->EndsScene()){
                while(!lKit->Queues[lKit->render_from].empty()){

                    if(!lKit->Queues[lKit->render_from].front()->IsPersistent())
                        delete lKit->Queues[lKit->render_from].front();

                    lKit->Queues[lKit->render_from].pop();
                }
                AdvanceRenderQueue<ThreadKit &>(*lKit);
            }
            lKit->monitor.Unlock();

            Op->Draw();

            if(!Op->IsPersistent()){
                delete Op;
                Op=nullptr;
            }

        }

        return nullptr;
    }

    void StartThread(Window *aWindow, const Version &aGLVersion){
        ThreadKit *lKit = new ThreadKit();
        lKit->render_from = 0;
        lKit->draw_to = 1;

        GetSystemThreadkit_internal() = lKit;

        ClaimWindow(aWindow);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_SCISSOR_TEST);

        printf(BRACKNAME " Info requesting OpenGL version %i.%i\n", aGLVersion.major, aGLVersion.minor);

        lKit->mWindow = new Window();
        lKit->mWindow->screen = aWindow->screen;
        lKit->mWindow->context= CreateContextFor(aWindow);
        ClaimWindow(aWindow);

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
    inline void ClaimWindow(Window *aClaim){
        SDL_GL_MakeCurrent(aClaim->screen, aClaim->context);
    }

    SDL_GLContext CreateContextFor(Window *aFor){

        SDL_GLContext lNewContext;
        SDL_GLContext lOldContext = SDL_GL_GetCurrentContext();
        SDL_Window *lOldWindow =SDL_GL_GetCurrentWindow();
        SetSDL_GL_Attributes();
        ClaimWindow(aFor);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        int profile, major, minor;
        if(!((SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile)==0) &&
             (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major)==0) &&
             (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor)==0))){
            printf(BRACKNAME " Error %s\n", SDL_GetError());
            assert(false); // Could not get info of original context. 
        }
        
        if(!((SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile)==0) &&
            (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major)==0) &&
            (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor)==0))){
            printf(BRACKNAME " Error %s\n", SDL_GetError());
            assert(false); // Could not set info of the new context. 
        }

        lNewContext = SDL_GL_CreateContext(aFor->screen);

        SDL_GL_MakeCurrent(lOldWindow, lOldContext);

        return lNewContext;

    }

}

}
}
