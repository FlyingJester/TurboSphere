#ifndef SAPPHIRE_INIT_GL_HEAD
#define SAPPHIRE_INIT_GL_HEAD

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#ifdef OPENGL_ES
    #include <SDL2/SDL_opengles2.h>
    #include <SDL2/SDL_opengles.h>
#else
    #include <SDL2/SDL_opengl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

inline void TS_GL_SetDefaultAttribs(void){

        SDL_GL_ResetAttributes();

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,  8);

        /////
        // TS works on Mesa again. Just let this be whatever it will.
        //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GL_TRUE);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,GL_TRUE);

        /////
        // TODO: Verify this works, then turn it up to 4.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);

        #ifdef OPENGL_ES
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
        #else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
        #endif

}

int InitGL(int sharelist, float scale, unsigned int width, unsigned int height);

#ifdef __cplusplus
}
#endif

#endif
