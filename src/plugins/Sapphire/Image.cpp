#include "Image.hpp"
#include <cassert>

#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <color.h>
#include <SDL2/SDL.h>
#include "Sapphire.hpp"

namespace Sapphire {

namespace Script{
}

namespace GL{

    inline void UploadTexture(unsigned w, unsigned h, void *pix){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
        #ifdef OS_X
        glFinish(); //Janky OS X OpenGL 4.
        #endif
    }
    template <typename T>
    inline void UploadTexture(T a, void *pix){UploadTexture(a->w, a->h, pix);}

    void Image::SetTexParameters(){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    Image::Image(){
        glGenTextures(1, &mTexture);
    }
    Image::~Image(){
        glDeleteTextures(1, &mTexture);
    }

    void Image::Bind() const{
        glBindTexture(GL_TEXTURE_2D, mTexture);
    }

    Image::Image(const SDL_Surface *aFrom){
        glGenTextures(1, &mTexture);
        Bind();
        SetTexParameters();

        UploadTexture(aFrom, aFrom->pixels);

    }

    Image::Image(unsigned aTexture, unsigned w, unsigned h){
        glGenTextures(1, &mTexture);
        Bind();
        SetTexParameters();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    }

    void Image::GetBuffer(void *aTo) const{
        Bind();
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aTo);

    }

}

Image::Image()
  : GL::Image() {

    assert( sizeof(PixelData) == sizeof (int32_t) );

    w = 0;
    h = 0;
    RGBA = nullptr;
}

Image::Image(const SDL_Surface *aFrom)
  : GL::Image(aFrom)
  , RGBA(new PixelData[aFrom->w*aFrom->h])
  , w(aFrom->w)
  , h(aFrom->h){

    assert( sizeof(PixelData) == sizeof (int32_t) );

    memcpy(RGBA, aFrom->pixels, aFrom->w*aFrom->h*4);

    printf(BRACKNAME " Info: created image.\n");
}

Image::Image(Image *aFrom)
  : GL::Image(aFrom->mTexture, aFrom->w, aFrom->h)
  , RGBA(new PixelData[aFrom->w*aFrom->h])
  , w(aFrom->w)
  , h(aFrom->h){

    assert( sizeof(PixelData) == sizeof (int32_t) );

    aFrom->GetBuffer(RGBA);

//    memcpy(RGBA, aFrom->RGBA, aFrom->BufferSize());

}

Image::~Image(){
    delete[] RGBA;
}

Image::PixelData *Image::Lock(){

    if(RGBA==nullptr){
        RGBA = new PixelData[w*h];
        GetBuffer(RGBA);
    }

    return RGBA;
}

void Image::Unlock(){
        Bind();
        GL::UploadTexture(w, h, RGBA);
}

void Image::CopyData(void *aTo){ //Fills a buffer with a copy of the color data.
    GetBuffer(aTo);
}

}
