#include "../Image.hpp"
#include <cassert>

#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <color.h>
#include <SDL2/SDL.h>
#include "../Sapphire.hpp"

#ifdef OS_X
//Janky OS X OpenGL 4. 
#define GL_TEX_UPLOAD_FINISH glFinish

#else

#define GL_TEX_UPLOAD_FINISH(...)

#endif

namespace Sapphire {

namespace GL {
    void UploadTexture(unsigned w, unsigned h, const void *pix){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
        GL_TEX_UPLOAD_FINISH();
    }
}

void Image::SetTexParameters(){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

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
    RGBA = static_cast<PixelData *>(malloc(sizeof(PixelData) * w * h));
    memcpy(RGBA, pixels, w*h*4);
    GL::UploadTexture(w, h, RGBA);
}

Image::Image(const SDL_Surface *aFrom)
  : Image(static_cast<uint32_t *>(aFrom->pixels), static_cast<unsigned>(aFrom->w), static_cast<unsigned>(aFrom->h)){
      
}

Image::Image(Image *aFrom)
  : Image(){
    
    w = aFrom->w; h = aFrom->h;
    RGBA = static_cast<PixelData *>(malloc(sizeof(PixelData) * w * h));

    aFrom->CopyData(RGBA);

    GL::UploadTexture(aFrom->w, aFrom->h, RGBA);
    
    printf(BRACKNAME " Info: Initialized image with texture %u with Image(Image *aFrom)\n", mTexture);
}

Image::~Image(){
    free(RGBA);
    glDeleteTextures(1, &mTexture);
}

void Image::Bind() const{
    glBindTexture(GL_TEXTURE_2D, mTexture);
}

Image::PixelData * &Image::Lock(){

    if(RGBA==nullptr){
        RGBA = static_cast<PixelData *>(malloc(sizeof(PixelData) * w * h));
        CopyData(RGBA);
    }

    return RGBA;
}

void Image::Unlock(unsigned w_, unsigned h_){
    w = w_;
    h = h_;
    Unlock();
}

void Image::Unlock(int x_, int y_, unsigned w_, unsigned h_){
    Bind();
    if((x_==0) && (y_==0) && (w_==w) && (h_==h)){
        Unlock();
    }
    else{
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_, y_, w_, h_, GL_RGBA, GL_UNSIGNED_BYTE, RGBA);
        GL_TEX_UPLOAD_FINISH();
    }
}

void Image::Unlock(){
        Bind();
        GL::UploadTexture(w, h, RGBA);
}

void Image::UpdateAt(int x_, int y_, unsigned w_, unsigned h_, const void *a){
    if((x_==0) && (y_==0) && (w_==w) && (h_==h)){
        Update(a);
        return;
    }
    else if((x_+w_<=0) || (y_+h_<=0)){
        return;
    }
    else if(x_+w_>w){
        for(int i = y_; (i<h_) && (i<h); i++){
            glTexSubImage2D(GL_TEXTURE_2D, 0, x_, i, w-x_, 1, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const uint32_t *>(a)+(w_*i));
        }
    }
    else if(y_+h_>h){
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_, y_, w_, h-y_, GL_RGBA, GL_UNSIGNED_BYTE, a);
    }
    else{
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_, y_, w_, h_, GL_RGBA, GL_UNSIGNED_BYTE, a);
    }
    GL_TEX_UPLOAD_FINISH();
}

void Image::CopyData(void *aTo){ //Fills a buffer with a copy of the color data.
    Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, aTo);
}

}
