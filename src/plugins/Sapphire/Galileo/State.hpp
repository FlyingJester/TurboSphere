#pragma once
#include "Shape.hpp"
#include "Shader.hpp"
#include "../Thread/Atomic.hpp"
#include <SDL2/SDL.h>
#include <screen.h>

// Contains Galileo::GL::Operations that do not do actual drawing.

namespace Sapphire{
namespace Galileo{

void SetFrameRate(int32_t aFramesPerSecond);
int32_t GetFrameRate(void);

class FlipScreen : public GL::Operation {
public:
    FlipScreen(SDL_Window *aWindow = nullptr);
    virtual ~FlipScreen(){}
    virtual int Draw();
    virtual bool IsPersistent(void){
        return false;
    }
    virtual bool EndsScene(){return true;}
};

class ClippingRectangle : public GL::Operation {
    static unsigned sx, sy, sw, sh;
public:
    const bool persistent;
    unsigned x, y, w, h;

    ClippingRectangle(bool p, unsigned x_, unsigned y_, unsigned w_, unsigned h_)
      : persistent(p)
      , x(x_), y(y_), w(w_), h(h_){
    }
    
    ClippingRectangle(bool p = true) 
      : ClippingRectangle(p, 0, 0, 0xFFFF, 0xFFFF){
    }
    
    virtual ~ClippingRectangle(){}
    virtual int Draw();
    virtual bool IsPersistent(void){
        return persistent;
    }

    static ClippingRectangle *FullScreen();

};

}
}
