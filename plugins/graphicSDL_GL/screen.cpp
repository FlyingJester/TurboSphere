#include "main.h"
#include "screen.h"

v8Function V8GetScreenWidth(V8ARGS){
    return v8::Number::New(GetScreenWidth());
}

v8Function V8GetScreenHeight(V8ARGS){
    return v8::Number::New(GetScreenHeight());
}

void FlipScreen(void){
    SDL_GL_SwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
}
