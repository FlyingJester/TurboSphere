#include "gl_init.h"
#include "../../../configmanager/opengame.h"
#include "../../../graphiccommon/screen.h"
#include <cassert>

#ifdef OPENGL_ES

    #include "gles_ortho.sup"

#else

    #include "gl_ortho.sup"

#endif


int InitGL(SDL_Window *window, int sharelist, float scale, unsigned int width, unsigned int height){

    static TS_Config * const TS_Conf = GetConfig();

    SDL_GLContext oldcontext = SDL_GL_GetCurrentContext();
    SDL_GLContext newcontext;

    if(window=NULL)
        window = SDL_GL_GetCurrentWindow();

    assert((SDL_Window *)window != NULL);

    TS_GL_SetDefaultAttribs();

    if(sharelist >= 0){

        if(oldcontext!=sharelist)
            SDL_GL_MakeCurrent(window, sharelist);

        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, GL_TRUE);

    }

    glcontext = SDL_GL_CreateContext(window);

    /////
    // TODO: Add VSync to TS_Config

//    int err = SDL_GL_SetSwapInterval(vsync);

    ResetOrtho();

    if(oldcontext!=sharelist){
        SDL_GL_MakeCurrent(window, oldcontext);
    }

    return context;
}

