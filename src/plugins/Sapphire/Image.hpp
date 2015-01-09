#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace Sapphire {

class Image{
public:
  union PixelData {
      uint32_t pixel;
      unsigned char channel[4];
  };

protected:
    
    static void SetTexParameters();
    
    unsigned mTexture;

    PixelData *RGBA;

    unsigned w;
    unsigned h;

public:

    Image();
    Image(const SDL_Surface *aFrom);
    Image(Image *aFrom);
    ~Image();

    unsigned Width() const {return w;}
    unsigned Height() const {return h;}

    PixelData *Lock();
    void Unlock();
    
    void Bind() const;

    size_t BufferSize() const {
      return w*h*4; //4 color channels at one byte each.
    }

    void CopyData(void *); //Fills a buffer with a copy of the color data.

    inline unsigned DebugGetTexture(){
        return mTexture;
    }
    
};

}
