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

  struct PixelDataInput {
      PixelDataInput(){}
      uint32_t operator() (const PixelData &that) const {
          return that.pixel;
      }
  };

  struct PixelDataOutput {
      PixelDataOutput(){}
      union PixelData operator() (uint32_t in) const {
          return {.pixel = in};
      }
  };
  
protected:
    
    static void SetTexParameters();
    
    unsigned mTexture;

    PixelData *RGBA;

    unsigned w;
    unsigned h;

public:

    Image();
    Image(const uint32_t *pixels, unsigned w_, unsigned h_);
    Image(const SDL_Surface *aFrom);
    Image(Image *aFrom);
    ~Image();

    unsigned Width() const {return w;}
    unsigned Height() const {return h;}

    PixelData * &Lock();
    void Unlock();
    void Unlock(unsigned w_, unsigned h_);
    inline void Reserve(unsigned w_, unsigned h_){
        if((w*h)<(w_*h_))
            RGBA = static_cast<PixelData *>(realloc(RGBA, w*h*sizeof(PixelData)));
    }
    
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
