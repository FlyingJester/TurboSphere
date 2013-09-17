#include "main.h"
#include "screen.h"

v8Function V8GetScreenWidth(V8ARGS){
    return v8::Number::New(GetScreenWidth());
}

v8Function V8GetScreenHeight(V8ARGS){
    return v8::Number::New(GetScreenHeight());
}

void FlipScreen(void){
        //Old SDL 1.2 code.
    //SDL_GL_SwapBuffers();

    SDL_GL_SwapWindow(screen);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
