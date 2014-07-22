#include "State.hpp"
#include "../Sapphire.hpp"
#include <OpenGL/gl3.h>

namespace Sapphire{
namespace Galileo{

FlipScreen::FlipScreen(atomic32_t *aRenderFrame)
  : mRenderFrame(aRenderFrame)
  , window(nullptr)
{

}

FlipScreen::FlipScreen(atomic32_t *aRenderFrame, SDL_Window *aWindow)
  : mRenderFrame(aRenderFrame)
  , window(aWindow)
{

}

int FlipScreen::Draw(){

    AtomicInc(mRenderFrame);
    //SDL_GL_SwapWindow(window);
    SDL_Window *w = SDL_GL_GetCurrentWindow();
    //printf(BRACKNAME " Info: Context is %p\tWindows at %p\n", SDL_GL_GetCurrentContext(), w);
    SDL_GL_MakeCurrent(w, SDL_GL_GetCurrentContext());
    SDL_GL_SwapWindow(w);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}

}
}
