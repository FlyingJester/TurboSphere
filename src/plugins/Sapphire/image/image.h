#ifndef SAPPHIRE_IMAGE_HEAD
#define SAPPHIRE_IMAGE_HEAD

#ifndef OPENGLES
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL_opengles.h>
#include <SDL2/SDL_opengles2.h>
#endif

#include "../loadfunc/gl_extra.h"
#include "../backend.h"
#include "../blendmode.h"

namespace Sapphire{

    /////
    // As prototyped in FJ-GL

    class image{
    public:

        image();
        ~image();

        /////
        // Do not call a non-trivial constructor from any thread but the main thread.
        image(image *);
        image(const image &);
        image(int width, int height, RGBA *pixels);
        image(int x, int y, int width, int height);

        void Blit(int x, int y, BlendMode blendmode);
        void BlitMask(int x, int y, BlendMode blendmode, RGBA mask, BlendMode mask_blendmode);
        void TransformBlit(int x[4], int y[4], BlendMode blendmode);
        void TransformBlitMask(int x[4], int y[4], BlendMode blendmode, RGBA mask, BlendMode mask_blendmode);
        RGBA* Lock(void);
        void Unlock(void);

        void (*freePixels)( RGBA *);
        RGBA *(*allocPixels)(size_t w, size_t h);

        RGBA *pixels;
        GLuint texture;
        unsigned int w;
        unsigned int h;
        GLuint TexCoordBuffer;
        GLuint ColorBuffer;
        GLuint VertexBuffer;
        GLuint VertexArray;
    };

}
#endif
