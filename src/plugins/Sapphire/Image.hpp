#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace Sapphire {

class Image;

  /////
  // Namespace GL contains the strongly OpenGL components.
/*
namespace GL{
  class Texture {
  protected:

      unsigned mTexture;

      Texture();
      Texture(const SDL_Surface *aFrom);
      Texture(unsigned aTexture, unsigned w, unsigned h);

      void GetBuffer(void *) const;

      /////
      // Sets default parameters on the currently bound image.
      
    
  public:
      /////
      // TODO: Pull this out of GL in the base, to ensure that we have at least ONE way to tell.
      virtual unsigned Height() const = 0;
      virtual unsigned Width() const = 0;
      
      virtual ~Texture();
      virtual void CopyData(void *) = 0; //Fills a buffer with a copy of the color data.
      
      virtual Image *AsImage() = 0;
      
  };
}
*/
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

    //void GLBind() const;
    //static void GLUnbind();

    unsigned Width() const {return w;}
    unsigned Height() const {return h;}

    PixelData *Lock();
    void Unlock();
    
    void Bind() const;

    size_t BufferSize() const {
      return w*h*4; //4 color channels.
    }

    void CopyData(void *); //Fills a buffer with a copy of the color data.

    virtual Image *AsImage(){
        return this;
    }
};

}
