#include "drawqueue.hpp"
#include "../common/concurrent_queue.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct TS_Renderer {
    SDL_GLContext *context;
    SDL_Window    *window;
};

typedef concurrent_queue<Sapphire::DrawingOp *> DrawOpQueue;

DrawOpQueue DrawQueue = DrawOpQueue();

static SDL_Thread *renderThread = NULL;
static SDL_GLContext threadglcontext;

int RenderThread(void *data) {

    int width, height;
    SDL_GLContext *context = ((TS_Renderer *)(data))->context;
    SDL_Window *window = ((TS_Renderer *)(data))->window;


    /////
    // This demo is designed to work even on Intel GMA cards.
    //
    // In fact, it was written on a machine with one.
    // The horror...the horror...
    //

    if(SDL_GL_MakeCurrent(window, *((SDL_GLContext*)context))!=0)
        SDL_ShowSimpleMessageBox(0, "Error", SDL_GetError(), NULL);

    SDL_GetWindowSize(window, &width, &height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);

    Sapphire::DrawingOp *operation = NULL;

    while(!GetThreadDie()){
        if(!DrawQueue.try_pop(operation)||(operation==NULL))
            continue;

        operation->call();

        delete operation;
        operation = NULL;

    }
    SetThreadDie(0);
    SDL_GL_DeleteContext(*((SDL_GLContext*)context));
    free(data);
    return 0;
}

void InitDrawQueue(void) {

    // Empty the queue, just in case.
    // Makes failures much more graceful on mismatched Init/Close calls.
    DrawQueue.clear();

    ResetRenderFrame();
    ResetEngineFrame();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    {
        SDL_GLContext oldcontext = SDL_GL_GetCurrentContext();
        if(!oldcontext)
            SDL_ShowSimpleMessageBox(0, "OpenGL Error", SDL_GetError(), NULL);

        SDL_ClearError();

        threadglcontext = SDL_GL_CreateContext(SDL_GL_GetCurrentWindow());

        {
            int ContextIsShared = 0;
            const int AttribErr = SDL_GL_GetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, &ContextIsShared);

            if(AttribErr)
            {
                SDL_ShowSimpleMessageBox(0, "OpenGL Error", SDL_GetError(), NULL);
                abort();
            }

            if(!ContextIsShared)
            {
                SDL_ShowSimpleMessageBox(0, "Error", "Could not make context shared.\nTurboSphere requires OpenGL 3.1 or greater.", NULL);
                abort();
            }
        }


        SDL_GL_MakeCurrent(SDL_GL_GetCurrentWindow(), oldcontext);
    }
    //SDL_ShowSimpleMessageBox(0, "Error", SDL_GetError(), NULL);
    {
        TS_Renderer* data = new TS_Renderer();
        data->context= &threadglcontext;
        data->window = SDL_GL_GetCurrentWindow();
        renderThread = SDL_CreateThread(RenderThread, "GLRenderer", data);
        if(!renderThread)
        {
            SDL_ShowSimpleMessageBox(0, "OpenGL Error", SDL_GetError(), NULL);
            abort();
        }
    }
}

void CloseDrawQueue(void) {
    SetThreadDie(1);
    while(GetThreadDie()==1){
        SDL_Delay(10);
    }

}

