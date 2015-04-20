#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace Sapphire {

// Not necessarily OpenGL...
namespace GL {
    
    void UploadTexture(unsigned w, unsigned h, const void *pix);
    
    template <typename T>
    inline void UploadTexture(const T a, const void *pix){UploadTexture(a->w, a->h, pix);}
    
    inline void UploadTexture(const SDL_Surface *s){UploadTexture(s, s->pixels);}
    
}

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
    // Specifies a new W and H. Useful with Reserve.
    void Unlock(unsigned w_, unsigned h_);
    // Indicates only the region inside the bounds has changed.
    void Unlock(int x_, int y_, unsigned w_, unsigned h_);
    
    inline void Reserve(unsigned w_, unsigned h_){
        if((w*h)<(w_*h_))
            RGBA = static_cast<PixelData *>(realloc(RGBA, w*h*sizeof(PixelData)));
    }

    void Bind() const;

    size_t BufferSize() const {
      return w*h*4; //4 color channels at one byte each.
    }

    void CopyData(void *); //Fills a buffer with a copy of the color data.

    inline unsigned GetRawTexture(){
        return mTexture;
    }

    inline unsigned DebugGetTexture(){
        return mTexture;
    }
    
    inline void Update(const void *a) const {
        Bind();
        GL::UploadTexture(w, h, a);
    }
    void UpdateAt(int x_, int y_, unsigned w_, unsigned h_, const void *a);

    inline void Update(SDL_Surface *a) const{SDL_LockSurface(a); Update(a->pixels); SDL_UnlockSurface(a);}

};

}
