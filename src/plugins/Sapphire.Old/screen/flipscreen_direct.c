#include "flipscreen.h"

/////
// Old fashioned.

void FlipScreenDirectInit(void){

}

void FlipScreenDirectClose(void){

}

void FlipScreenDirect(void){
    SDL_GL_SwapWindow(screen);
    glClear(GL_COLOR_BUFFER_BIT);
}
