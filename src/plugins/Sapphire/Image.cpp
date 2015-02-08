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
    namespace GL{

        inline void UploadTexture(unsigned w, unsigned h, void *pix){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
            #ifdef OS_X
            glFinish(); //Janky OS X OpenGL 4.
            #endif
        }
        template <typename T>
        inline void UploadTexture(T a, void *pix){UploadTexture(a->w, a->h, pix);}
        
    }
    
    void Image::SetTexParameters(){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    
/*


    Texture::Texture(){
        glGenTextures(1, &mTexture);
    }
    Texture::~Texture(){
        glDeleteTextures(1, &mTexture);
    }


    Texture::Texture(const SDL_Surface *aFrom){
        glGenTextures(1, &mTexture);
        Bind();
        SetTexParameters();

        UploadTexture(aFrom, aFrom->pixels);

    }

    Texture::Texture(unsigned aTexture, unsigned w, unsigned h){
        glGenTextures(1, &mTexture);
        Bind();
        SetTexParameters();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    }

    void Texture::GetBuffer(void *aTo) const{
        Bind();
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aTo);

    }

}
*/

Image::Image(){
    
    static unsigned num = 0;
    num++;
    
    glGenTextures(1, &mTexture);
    Bind();
    SetTexParameters();

    assert( sizeof(PixelData) == sizeof (int32_t) );

    w = 0;
    h = 0;
    RGBA = nullptr;
    
    printf(BRACKNAME " Info: Initialized image %u with texture %u with Image()\n",  num, mTexture);
    
}

Image::Image(const uint32_t *pixels, unsigned w_, unsigned h_)
  : Image(){
    w = w_;
    h = h_;
    RGBA = new PixelData[w*h];
    memcpy(RGBA, pixels, w*h*4);
    GL::UploadTexture(w, h, RGBA);
}

Image::Image(const SDL_Surface *aFrom)
  : Image(static_cast<uint32_t *>(aFrom->pixels), static_cast<unsigned>(aFrom->w), static_cast<unsigned>(aFrom->h)){
      
}

Image::Image(Image *aFrom)
  : Image(){
    
    w = aFrom->w; h = aFrom->h;
    RGBA = new PixelData[w*h];

    aFrom->CopyData(RGBA);

    GL::UploadTexture(aFrom->w, aFrom->h, RGBA);
    
    printf(BRACKNAME " Info: Initialized image with texture %u with Image(Image *aFrom)\n", mTexture);
}

Image::~Image(){
    delete[] RGBA;
    glDeleteTextures(1, &mTexture);
}

void Image::Bind() const{
    glBindTexture(GL_TEXTURE_2D, mTexture);
}

Image::PixelData *Image::Lock(){

    if(RGBA==nullptr){
        RGBA = new PixelData[w*h];
        CopyData(RGBA);
    }

    return RGBA;
}

void Image::Unlock(){
        Bind();
        GL::UploadTexture(w, h, RGBA);
}

void Image::CopyData(void *aTo){ //Fills a buffer with a copy of the color data.
    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aTo);
}

}
