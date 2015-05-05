#pragma once

#include "Galileo/Galileo.hpp"
#include <SDL2/SDL.h>
#include "Thread/Atomic.hpp"
#include "libyyymonitor/monitor.hpp"
#include "Thread/Thread.hpp"
#include "Galileo/Shape.hpp"
#include <queue>

#ifdef CreateWindow
#undef CreateWindow
#endif

// This is a weird constant.
//
// A higher number of buffers will always result in either higher latency or lower framerates, depending on if the render or engine threads is running faster.
// A lower number increases the CPU usage. Even numbers cause the render and engine threads to keep leapfroggin each other. If you can reason why, let me know.
// 3 is the lowest sensible number to have. It's certainly nice to use more, but if you are on a platform without JS JIT or usign a debug JS library, there 
//   will be blood from 5 or more.
#define NUM_BUFFERS 3

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS|SDL_WINDOW_ALLOW_HIGHDPI

namespace Sapphire{
namespace GL{

template<int T>
class DummyOperation : public Sapphire::Galileo::GL::Operation {
    virtual int Draw(){return T;}
};

template<typename T>
void AdvanceRenderQueue(T kit){
    assert(kit.render_from!=kit.draw_to);
    do{
        kit.render_from++;
        kit.render_from%=NUM_BUFFERS;
    }while(kit.draw_to==kit.render_from);
    assert(kit.render_from!=kit.draw_to);
}

template<typename T>
void AdvanceDrawQueue(T kit){
    assert(kit.render_from!=kit.draw_to);
    do{
        kit.draw_to++;
        kit.draw_to%=NUM_BUFFERS;
    }while(kit.draw_to==kit.render_from);
    assert(kit.render_from!=kit.draw_to);
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
    unsigned char major, minor;
};

SDL_GLContext CreateForWindow(Window *aFor, const Version &aGLVersion);

namespace MainThread{

    // Creates an OpenGL window of aWidth x aHeight, with OpenGL version aGLVersion.
    Window *CreateWindow(unsigned aWidth, unsigned aHeight, const Version &aGLVersion);
}

namespace RenderThread{

    // Starts the render thread on said window.
    void StartThread(Window *aWindow, const Version &aGLVersion);
    void StopThread();

    // Makes the given window available to the current thread. This is necessary
    // in order to create a context that is shared with the on associated with the
    // window.
    void ClaimWindow(Window *aClaim);
    // Uses the same version and profile as the original context attached to aFor.
    SDL_GLContext CreateContextFor(Window *aFor);

}

}
}
