#ifndef SAPPHIRE_IMAGE_HEAD
#define SAPPHIRE_IMAGE_HEAD

#include "sapphire.h"
#include "fjgl.h"
#include "../common/plugin.h"

#include <t5.h>

#include <GL/gl.h>
#include <SDL2/SDL_atomic.h>
#include <SDL2/SDL_surface.h>
#include <cassert>

namespace Sapphire{

    class Image;

    extern Turbo::JSObj<Image> ImageObject;

    namespace Script {
        Turbo::JSFunction CreateImage(Turbo::JSArguments args);
    }

    void InitImage(uint32_t ID, v8::Isolate *isol);

    /////
    // Holder for OpenGL textures.
    //
    // Every Image must have a unique OpenGL Texture associated with it,
    //  unless the texture is zero.
    //
    // An image that is persistent will NOT be destroyed when its reference
    //  count reaches zero. This is used for some internal images (such as the
    //  Screen image for compositing).
    //
    // Otherwise, an Image will be destroyed when DecImageRef() is called on
    //  it and this lowers its reference count to 0. This is indicated by a
    //  return value of 1 from DecImageRef().
    //

    class Image {
    public:
        friend int MakeImageWeak(Image *im);
        friend int MakeImagePersistent(Image *im);
        friend int IncImageRef(Image *im);
        friend int DecImageRef(Image *im);

        Image(void){
            texture = 0;
            width = 0;
            height = 0;

            persistent = 0;
            SDL_AtomicSet(&refs, 0);
        }

        Image(GLuint _t, unsigned int _w, unsigned int _h){
            texture = _t;
            width  = _w;
            height = _h;

            persistent = 0;
            SDL_AtomicSet(&refs, 0);

            pixels = NULL;
        }

        Image(RGBA *, unsigned int _w, unsigned int _h, bool owns_pixels = false);
        ~Image();

        GLuint GetTexture(void){
            return texture;
        }
        GLuint GetWidth(void){
            return width;
        }
        GLuint GetHeight(void){
            return height;
        }


        inline int IsPersistent(void){
            return persistent;
        }

    private:

        SDL_atomic_t refs;

        int persistent;
        GLuint texture;
        unsigned int width;
        unsigned int height;

        RGBA *pixels;

    };

    inline int MakeImagePersistent(Image *im){
        im->persistent = true;
        return 0;
    }

    inline int MakeImageWeak(Image *im){
        if(SDL_AtomicGet(&(im->refs))<=0){
            delete im;
            return 1;
        }
        im->persistent = false;
        return 0;
    }

    inline int IncImageRef(Image *im) {assert(im); SDL_AtomicIncRef(&(im->refs)); return 0;}
    inline int DecImageRef(Image *im) {assert(im); if((SDL_AtomicAdd(&(im->refs), -1)<=1)&&!im->IsPersistent()){delete im; return 1;} else {return 0;}}

}

#endif
