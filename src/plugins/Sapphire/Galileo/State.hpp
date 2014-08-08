#pragma once
#include "Shape.hpp"
#include "Shader.hpp"
#include "../Thread/Atomic.hpp"
#include <SDL2/SDL.h>

// Contains Galileo::GL::Operations that do not do actual drawing.

namespace Sapphire{
namespace Galileo{

void SetFrameRate(int32_t aFramesPerSecond);
int32_t GetFrameRate(void);

class FlipScreen : public GL::Operation {
    atomic32_t *mRenderFrame;
    SDL_Window *window;

public:
    FlipScreen(atomic32_t *aRenderFrame);
    FlipScreen(atomic32_t *aRenderFrame, SDL_Window *aWindow);
    virtual int Draw();
    virtual bool IsPersistent(void){
        return false;
    }
};

}
}
