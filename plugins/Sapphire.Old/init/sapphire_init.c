
#include <stdbool.h>

#include <SDL2/SDL.h>

#ifdef OPENGL_ES
    #include <SDL2/SDL_opengles2.h>
    #include <SDL2/SDL_opengles.h>
#else
    #include <SDL2/SDL_opengl.h>
#endif

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#include "../backend.h"
#include "../loadfunc/gl_extra.h"
#include "gl_init.h"

#include "../image/image.h"

SDL_Window *screen = NULL;

RGBA *ScreenCopy = NULL;

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288 //Stolen from SDL
#endif

float SPI = M_PI;

GLuint CurrentShader = 0;
GLuint DefaultShader = 0;

unsigned int Scale = 1;
unsigned int Width = 320;
unsigned int Height= 240;

int ClipRectX, ClipRectY, ClipRectW, ClipRectH;

GLuint EmptyTexture = 0;

GLuint TexCoordBuffer = 0;
GLuint ZeroTexCoordBuffer = 0;
GLuint FullColorBuffer = 0;
GLuint SeriousCoordBuffer = 0;

GLuint VertexAttrib = 0;
GLuint ColorAttrib = 0;
GLuint TexCoordAttrib = 0;

SDL_GLContext maincontext = NULL;

GLint GetVertexAttrib(void){
    return VertexAttrib;
}
GLint GetColorAttrib(void){
    return ColorAttrib;
}
GLint GetTexCoordAttrib(void){
    return TexCoordAttrib;
}

void SetClippingRectangle(int x, int y, int w, int h){

    if((unsigned)w>Width)
        w=(int)Width;
    if((unsigned)h>Height)
        w=(int)Width;
    if((unsigned)x<0)
        x=0;
    if((unsigned)y<0)
        y=0;

    if((x==ClipRectX)&&(y==ClipRectY)&&(w==ClipRectW)&&(h==ClipRectH))
        return;

    ClipRectX = x;
    ClipRectY = y;
    ClipRectW = w;
    ClipRectH = h;

    glScissor(x, Height-h-y, w, h);

}
void GetClippingRectangle(int* x, int* y, int* w, int* h){

    *x = ClipRectX;
    *y = ClipRectY;
    *w = ClipRectW;
    *h = ClipRectH;

}

/////
// Set the window height and width before this function call.

bool InitVideo(int w, int h, Configuration_t *config){

    assert( config != NULL );

    assert( w > 0 );
    assert( h > 0 );

/////
// Setup Shorthand constants.

    GLint ScreenWidth;
    GLint ScreenHeight;

    uint32_t white = 0xFFFFFFFF;

    bool fullscreen = configl.fullscreen;

    if(!configl.niceCircles){
        printf("[FJ-GL] Using faster linear approximations.\n");
        SPI = M_PI/2.0f;
    }

    GLfloat texcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    GLfloat fullcolor[] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    GLfloat stexcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat ztexcoords[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    Width = w;
    Height = h;

    Scale = configl.scale;

    ClipRectX = 0;
    ClipRectY = 0;
    ClipRectW = w*Scale;
    ClipRectH = h*Scale;

    SetClippingRectangle(0, 0, w*Scale, h*Scale);

    ScreenCopy = realloc(ScreenCopy, 4*w*h);


    /////
    // Create Window


    if(
        (screen = SDL_CreateWindow("TurboSphere Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  w*config->scale, h*config->scale,
                                  SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS|((config->fullscreen)?SDL_WINDOW_FULLSCREEN:0))
        )==NULL)
    {
        printf("[" PLUGINNAME "] Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }

    /////
    // Create Main OpenGL context.




    maincontext = InitGL(screen, NULL, configl.scale, w, h);

    /////
    // Make the screen black

    glClear(GL_COLOR_BUFFER_BIT);

    /////
    // Load basic functions.

    LoadGLFunctions();

    /////
    // Set GL variables

    int err = SDL_GL_SetSwapInterval(configl.vsync);

    if(err<0){
        printf("[" PLUGINNAME "] Init Warning: preferred vsync mode (%s) is not supported.\n", (configl.vsync==0)?((const char *)"vsync off"):((const char *)"vsync on"));
    }
/*
    glClearColor(0, 0, 0, 255);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

*/
    printf("[" PLUGINNAME "] Info: Using OpenGL version %s\n", glGetString(GL_VERSION));
    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("[" PLUGINNAME "] Unable to init SDL Image: %s\n", IMG_GetError());
    }

	SDL_ShowCursor(0);

/////
// Create Main OpenGL COntext.




/*
    VertexAttrib = glGetAttribLocation(CurrentShader, "Vertex");
    ColorAttrib = glGetAttribLocation(CurrentShader, "Color");
    TexCoordAttrib = glGetAttribLocation(CurrentShader, "TexCoord");
    printf("[FJ-GL] Using Vertex Arrays.\n");

    if(configl.niceImages){
        printf("[FJ-GL] Using nicest video settings.\n");
        glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST); //Unused in FJ-GL, but included for completeness because it does no harm.
    }
    else {
        printf("[FJ-GL] Using fastest video settings.\n");
        glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_FASTEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
        glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST); //Unused in FJ-GL, but included for completeness because it does no harm.
    }

    glDepthMask(GL_FALSE);
    glDisable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glGenTextures(1, &EmptyTexture);
    glBindTexture(GL_TEXTURE_2D, EmptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);


    glGenBuffers(1, &TexCoordBuffer);
    glAlphaFunc(GL_NOTEQUAL, 0.0f);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, texcoords, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    //GLuint fullcolor[] = {0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu};

*/

    glGenBuffers(1, &FullColorBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, FullColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, fullcolor, GL_STATIC_DRAW);

    glGenBuffers(1, &SeriousCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, SeriousCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)<<4, stexcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &ZeroTexCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ZeroTexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)<<3, ztexcoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);



    return true;

}
