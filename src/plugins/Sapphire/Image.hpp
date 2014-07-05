#pragma once

#include <SDL2/SDL.h>

namespace Sapphire {

  /////
  // Namespace GL contains the strongly OpenGL components.
namespace GL{
  class Image {
  protected:

      unsigned mTexture;

      Image(){}
      Image(const SDL_Surface *aFrom);
      Image(unsigned aTexture, unsigned w, unsigned h);

      void GetBuffer(void *);

  public:
      virtual ~Image();
      virtual void Bind() const;
  };
}

class Image : public GL::Image {
public:
  union PixelData {
      uint32_t pixel;
      unsigned char channel[4];
  };

protected:

    PixelData *RGBA;

    unsigned w;
    unsigned h;

public:
    Image();
    Image(const SDL_Surface *aFrom);
    Image(Image *aFrom);
    virtual ~Image();

    //void GLBind() const;
    //static void GLUnbind();

    inline unsigned Width() const  {return w;}
    inline unsigned Height() const {return h;}

    PixelData *LockImage();
    void UnlockImage();

    size_t BufferSize() const {
      return w*h*4; //4 color channels.
    }

    void CopyData(void *); //Fills a buffer with a copy of the color data.

};

}
