#include "../Image.hpp"
#include <cassert>

#include <color.h>
#include <SDL2/SDL.h>
#include "../Sapphire.hpp"


namespace Sapphire {

namespace GL {
    void UploadTexture(unsigned int a, unsigned int b, void const* c){
        assert(c);
    }
}

void Image::SetTexParameters() {}

Image::Image(){
    RGBA = nullptr;
    w = 0;
    h = 0;
}

Image::Image(const uint32_t *pixels, unsigned w_, unsigned h_)
  : Image(){
    w = w_;
    h = h_;
    assert(sizeof(PixelData)==4); // This needs to be true.
    RGBA = static_cast<PixelData *>(malloc(BufferSize()));
    if(pixels)
        memcpy(RGBA, pixels, BufferSize());
}

Image::Image(const SDL_Surface *aFrom)
  : Image(static_cast<uint32_t *>(aFrom->pixels), aFrom->w, aFrom->h){

}

Image::Image(Image *aFrom)
  : Image((uint32_t *)(aFrom->Lock()), aFrom->Width(), aFrom->Height()){
  
}

Image::~Image(){
    free(RGBA);
}

Image::PixelData * &Image::Lock(){
    return RGBA;
}

void Image::Unlock(){}
    // Specifies a new W and H. Useful with Reserve.
void Image::Unlock(unsigned w_, unsigned h_){
    w = w_;
    h = h_;
    RGBA = static_cast<PixelData *>(realloc(RGBA, BufferSize()));
}
    // Indicates only the region inside the bounds has changed.
void Image::Unlock(int x_, int y_, unsigned w_, unsigned h_){
    bool changed = false;

    if(x_+w_>w){
        w = x_+w_;
        changed = true;
    }
    if(y_+h_>h){
        h = y_+h_;
        changed = true;
    }
    
    if(changed)
        RGBA = static_cast<PixelData *>(realloc(RGBA, BufferSize()));
    
}

void Image::Bind() const {}

void Image::CopyData(void *a){ //Fills a buffer with a copy of the color data.
    memcpy(a, RGBA, BufferSize());
}

void Image::UpdateAt(int x_, int y_, unsigned w_, unsigned h_, const void *a){
    if((x_==0) && (y_==0) && (w_==w) && (h_==h)){
        Update(a);
        return;
    }
    else if((x_+w_<=0) || (y_+h_<=0)){
        return;
    }
    else {
        int x_from = x_, x_to = x_+w_,
            y_from = y_, y_to = y_+h_;
        unsigned x_inset = 0, y_inset = 0;
        
        if(x_from < 0){
            x_inset = -x_from;
            x_from = 0;
        }
        if(x_to>w)
            x_to = w;
        if(y_from < 0){
            y_inset = -y_from;
            y_from = 0;
        }
        if(y_to>h)
            y_to = h;

        assert(x_to>x_from);
        assert(y_to>y_from);
        const unsigned x_range = x_to-x_from,
            y_range = y_to-y_from;
        
        for(int y = 0; y<y_range; y++){
            memcpy(((uint32_t *)RGBA)+x_from+((y+y_from)*w), ((uint32_t *)a)+(y*w_), x_range*4);
        }
        
    }
}

}
