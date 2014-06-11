#include "gl_init.h"
//#include "../../../configmanager/opengame.h"

#include "../../../graphiccommon/screen.h"

#include <assert.h>

#ifdef OPENGL_ES

    #include "gles_ortho.sup"

#else

    #include "gl_ortho.sup"

#endif


SDL_GLContext InitGL(SDL_Window *window, SDL_GLContext sharelist, float scale, unsigned int width, unsigned int height){

    SDL_GLContext oldcontext = SDL_GL_GetCurrentContext();

    if(window==NULL)
        window = SDL_GL_GetCurrentWindow();

    assert((SDL_Window *)window != NULL);

    TS_GL_SetDefaultAttribs();

    if(sharelist != NULL){

        if(oldcontext!=sharelist)
            SDL_GL_MakeCurrent(window, sharelist);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, GL_TRUE);

    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

    /////
    // TODO: Add VSync to TS_Config

//    int err = SDL_GL_SetSwapInterval(vsync);

    ResetOrtho(1.0f);

    if(oldcontext!=sharelist){
        SDL_GL_MakeCurrent(window, oldcontext);
    }

    return context;
}

