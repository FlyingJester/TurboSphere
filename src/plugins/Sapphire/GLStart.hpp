#pragma once

#include "Galileo/Galileo.hpp"
#include <SDL2/SDL.h>
#include "Thread/Atomic.hpp"
#include "libyyymonitor/monitor.hpp"
#include "Thread/Thread.hpp"
#include "Galileo/Shape.hpp"
#include <queue>

#define NUM_BUFFERS 16

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS|SDL_WINDOW_ALLOW_HIGHDPI

namespace Sapphire{
namespace GL{

//concurrent_queue<Sapphire::Galileo::GL::Operation *> *RenderQueue();

//void SwapQueues();

template<int T>
class DummyOperation : public Sapphire::Galileo::GL::Operation {
    virtual int Draw(){return T;}
};

template<typename T>
void AdvanceRenderQueue(T kit){
    kit.render_from++;
    kit.render_from%=NUM_BUFFERS;
    if(kit.render_from==kit.draw_to){
        kit.render_from++;
        kit.render_from%=NUM_BUFFERS;
    }
}

template<typename T>
void AdvanceDrawQueue(T kit){
    kit.draw_to++;
    kit.draw_to%=NUM_BUFFERS;

    if(kit.draw_to==kit.render_from){
        kit.draw_to++;
        kit.draw_to%=NUM_BUFFERS;
        while(!kit.Queues[kit.draw_to].empty())
            kit.Queues[kit.draw_to].pop();
    }
}

struct Window {
    SDL_GLContext context;
    SDL_Window   *screen;
};

struct ThreadKit{ // A recursive structure that describes and controls a renderthread.

    Window *mWindow; // Includes the destined context, not the creating thread's context.
    atomic32_t *ShouldDie;
    atomic32_t *DidDie;
    TS_Thread  *Thread;

    std::queue<Sapphire::Galileo::GL::Operation *> Queues[NUM_BUFFERS];

    Turbo::Monitor monitor;

    int draw_to, render_from;

};

struct ThreadKit *GetSystemThreadkit(void);

// OpenGL version.
struct Version{
    char major;
    char minor;
};

SDL_GLContext CreateForWindow(Window *aFor, Version aGLVersion);

namespace MainThread{

    // Creates an OpenGL window of aWidth x aHeight, with OpenGL version aGLVersion.
    Window *CreateWindow(unsigned aWidth, unsigned aHeight, Version aGLVersion);

}

namespace RenderThread{

    // Starts the render thread on said window.
    void StartThread(Window *aWindow);
    void StopThread();

    // Makes the given window available to the current thread. This is necessary
    // in order to create a context that is shared with the on associated with the
    // window.
    void ClaimWindow(Window *aClaim);
    SDL_GLContext CreateContextFor(Window *aFor, Version aGLVersion);

}

}
}
