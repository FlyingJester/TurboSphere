#pragma once

#include "Galileo/Galileo.hpp"
#include <SDL2/SDL.h>
#include "Thread/Atomic.hpp"
#include "Thread/Thread.hpp"
#include "Galileo/Shape.hpp"
#include <TSPR/concurrent_queue.h>

#define NUM_BUFFERS 16

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS|SDL_WINDOW_ALLOW_HIGHDPI

namespace Sapphire{
namespace GL{

concurrent_queue<Sapphire::Galileo::GL::Operation *> *RenderQueue();

void SwapQueues();

template<int T>
class DummyOperation : public Sapphire::Galileo::GL::Operation {
    virtual int Draw(){return T;}
};

struct Window {
    SDL_GLContext context;
    SDL_Window   *screen;
};

struct ThreadKit{ // A recursive structure that describes and controls a renderthread.

    Window *mWindow; // Includes the destined context, not the creating thread's context.
    atomic32_t *ShouldDie;
    atomic32_t *DidDie;
    TS_Thread  *Thread;

    concurrent_queue<Sapphire::Galileo::GL::Operation *> Queues[NUM_BUFFERS];

    TS_Atomic32 *index;
    TS_Atomic32 *lastIndex;
};

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
