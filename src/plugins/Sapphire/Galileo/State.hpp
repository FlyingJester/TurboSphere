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
    SDL_Window *window;

public:
    FlipScreen(SDL_Window *aWindow = nullptr);
    virtual int Draw();
    virtual bool IsPersistent(void){
        return false;
    }
    virtual bool EndsScene(){return true;}
};

}
}
