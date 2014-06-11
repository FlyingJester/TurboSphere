#include "gl_extra.h"
#define GL_GLEXT_PROTOTYPES 1

#ifdef __linux__
#ifdef OPENGL_ES
    #include <SDL2/SDL_opengles2.h>
#else
    #include <SDL2/SDL_opengl.h>
#endif

#include <SDL2/SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define rmask 0xff000000
    #define gmask 0x00ff0000
    #define bmask 0x0000ff00
    #define amask 0x000000ff
#else
    #define rmask 0x000000ff
    #define gmask 0x0000ff00
    #define bmask 0x00ff0000
    #define amask 0xff000000
#endif
#else

//Very, very little-endian.

    #define rmask 0x000000ff
    #define gmask 0x0000ff00
    #define bmask 0x00ff0000
    #define amask 0xff000000

#endif

#include <stdio.h>

#define CHANNEL_MASKS rmask, gmask, bmask, amask

void (APIENTRY * glEnableVertexAttribArray)(GLuint) = NULL;
void (APIENTRY * glDisableVertexAttribArray)(GLuint) = NULL;
void (APIENTRY * glGenBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindBuffer)(GLenum,  GLuint) = NULL;
void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (APIENTRY * glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (APIENTRY * glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;
GLenum (APIENTRY * glCreateShader)(GLenum) = NULL;
void (APIENTRY * glDeleteShader)(GLenum) = NULL;
void (APIENTRY * glShaderSource)(GLenum, GLint, const GLchar **, const GLint *) = NULL;
void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*) = NULL;
void (APIENTRY * glCompileShader)(GLenum) = NULL;
GLenum (APIENTRY * glCreateProgram)(void) = NULL;
void (APIENTRY * glUseProgram)(GLenum) = NULL;
bool (APIENTRY * glIsProgram)(GLuint) = NULL;
void (APIENTRY * glAttachShader)(GLenum, GLenum) = NULL;
void (APIENTRY * glLinkProgram)(GLenum) = NULL;
void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*) = NULL;
GLboolean (APIENTRY * glIsShader)(GLuint) = NULL;
void (APIENTRY * glGetShaderInfoLog)(GLuint,  GLsizei,  GLsizei *,  GLchar *) = NULL;
void (APIENTRY * glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) = NULL;
void (APIENTRY * glDeleteProgram)(GLuint) = NULL;
GLint(APIENTRY * glGetUniformLocation)(GLuint program, const GLchar *name) = NULL;
void (APIENTRY * glProgramUniform1f)(GLuint program, GLint location, GLfloat v0) = NULL;
void (APIENTRY * glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (APIENTRY * glGenerateMipmap)(GLenum) = NULL;
void (APIENTRY * glGenFramebuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteFramebuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindFramebuffer)(GLenum, GLuint) = NULL;
void (APIENTRY * glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;

void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindVertexArray)(GLuint) = NULL;
GLint (APIENTRY * glGetAttribLocation)(GLuint, const GLchar *) = NULL;
GLint (APIENTRY * glVertexAttribPointer)(GLuint name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *data) = NULL;


#ifdef _WIN32
void (APIENTRY * glBlendEquation)(GLenum) = NULL;
#endif
void APIENTRY TS_CopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel,
	    GLint srcX, GLint srcY, GLint srcZ,
	    GLuint dstName, GLenum dstTarget, GLint dstLevel,
	    GLint dstX, GLint dstY, GLint dstZ,
	    GLsizei width, GLsizei height, GLsizei depth);
#ifdef __linux__
#define CHECK_FOR_PROCESS(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        fprintf(stderr, "[FJ-GL] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
}
#define CHECK_FOR_PROCESS_NON_FATAL(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        fprintf(stderr, "[FJ-GL] Init Error: " NAME " is not present in OpenGL library.\n");\
    }\
}
#else
#define CHECK_FOR_PROCESS(NAME){\
        if(wglGetProcAddress(NAME)==NULL){\
        fprintf(stderr, "[FJ-GL] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
}

#define CHECK_FOR_PROCESS_NON_FATAL(NAME){\
        if(wglGetProcAddress(NAME)==NULL){\
        fprintf(stderr, "[FJ-GL] Init Error: " NAME " is not present in OpenGL library.\n");\
    }\
}

#endif


#ifdef __linux__
#define GET_GL_FUNCTION(NAME, TYPING)\
CHECK_FOR_PROCESS( #NAME );\
NAME = TYPING SDL_GL_GetProcAddress( #NAME )
#define GET_GL_FUNCTION_NON_FATAL(NAME, TYPING)\
CHECK_FOR_PROCESS_NON_FATAL( #NAME );\
NAME = TYPING SDL_GL_GetProcAddress( #NAME )
#else
#define GET_GL_FUNCTION(NAME, TYPING)\
CHECK_FOR_PROCESS( #NAME );\
NAME = TYPING wglGetProcAddress( #NAME )
#define GET_GL_FUNCTION_NON_FATAL(NAME, TYPING)\
CHECK_FOR_PROCESS_NON_FATAL( #NAME );\
NAME = TYPING wglGetProcAddress( #NAME )

#endif


void LoadGLFunctions(void){
    static int first = 0;

    if(first)
        return;
    first = 1;

    GET_GL_FUNCTION(glEnableVertexAttribArray,  (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glDisableVertexAttribArray, (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGenBuffers,               (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteBuffers,            (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindBuffer,               (void (APIENTRY *)(GLenum, GLuint)));
    GET_GL_FUNCTION(glBufferData,               (void (APIENTRY *)(GLenum, GLsizeiptr, const GLvoid *, GLenum)));
    GET_GL_FUNCTION(glBufferSubData,            (void (APIENTRY *)(GLenum, GLintptr, GLsizeiptr, const GLvoid *)));
    GET_GL_FUNCTION(glCreateShader,           (GLenum (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glDeleteShader,             (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glShaderSource,             (void (APIENTRY *)(GLuint, GLsizei, const GLchar **, const GLint *)));
    GET_GL_FUNCTION(glGetShaderiv,              (void (APIENTRY *)(GLuint, GLenum, GLint *)));
    GET_GL_FUNCTION(glCompileShader,            (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glCreateProgram,          (GLenum (APIENTRY *)(void)));
    GET_GL_FUNCTION(glUseProgram,               (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glIsProgram,                (bool (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glAttachShader,             (void (APIENTRY *)(GLenum,  GLenum)));
    GET_GL_FUNCTION(glLinkProgram,              (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glGetProgramiv,             (void (APIENTRY *)(GLuint, GLenum, GLint*)));
    GET_GL_FUNCTION(glIsShader,            (GLboolean (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetShaderInfoLog,         (void (APIENTRY *)(GLuint,  GLsizei,  GLsizei *,  GLchar *)));
    GET_GL_FUNCTION(glGetProgramInfoLog,        (void (APIENTRY *)(GLuint, GLsizei, GLsizei*, GLchar*)));
    GET_GL_FUNCTION(glDeleteProgram,            (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetUniformLocation,      (GLint (APIENTRY *)(GLuint, const GLchar *)));
    GET_GL_FUNCTION(glProgramUniform1f,         (void (APIENTRY *)(GLuint program, GLint location, GLfloat v0)));
    GET_GL_FUNCTION(glBlendFuncSeparate,        (void (APIENTRY *)(GLenum, GLenum, GLenum, GLenum)));
    GET_GL_FUNCTION(glGenerateMipmap,           (void (APIENTRY *)(GLenum)));

    GET_GL_FUNCTION(glGenVertexArrays,          (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteVertexArrays,       (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindVertexArray,          (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetAttribLocation,       (GLint (APIENTRY *)(GLuint, const GLchar*)));
    GET_GL_FUNCTION(glVertexAttribPointer,     (GLint (APIENTRY *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*)));
    GET_GL_FUNCTION(glGenFramebuffers,          (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteFramebuffers,       (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindFramebuffer,          (void (APIENTRY *)(GLenum, GLuint)));
    GET_GL_FUNCTION(glFramebufferTexture2D,     (void (APIENTRY *)(GLenum, GLenum, GLenum, GLuint, GLint)));

#ifdef __linux__
    if(SDL_GL_GetProcAddress("glCopyImageSubData")!=NULL){
        glCopyImageSubData = (void(APIENTRY *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)) SDL_GL_GetProcAddress("glCopyImageSubData");
    }
    else
    if(SDL_GL_GetProcAddress("glCopyImageSubDataNV")!=NULL){
        glCopyImageSubData = (void(APIENTRY *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)) SDL_GL_GetProcAddress("glCopyImageSubDataNV");
        fprintf(stderr, "[FJ-GL] Init Warning: glCopyImageSubData is not present in OpenGL library. Using deprecated hardware fallback.\n");
    }
    else{
        glCopyImageSubData = TS_CopyImageSubData;
        fprintf(stderr, "[FJ-GL] Init Warning: glCopyImageSubData is not available, and hardware fallbacks are not available. Emulating in software.\n");
    }
#else
    GET_GL_FUNCTION(glBlendEquation,        (void (APIENTRY *)(GLenum)));
#endif

}

#ifndef _WIN32
void TS_CopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel,
	    GLint srcX, GLint srcY, GLint srcZ,
	    GLuint dstName, GLenum dstTarget, GLint dstLevel,
	    GLint dstX, GLint dstY, GLint dstZ,
	    GLsizei width, GLsizei height, GLsizei depth){

    int srcwidth, srcheight, dstwidth, dstheight;
    glBindTexture(GL_TEXTURE_2D, dstName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dstwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dstheight);
    glBindTexture(GL_TEXTURE_2D, srcName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &srcwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &srcheight);

    if(!((srcwidth==width)&&(srcheight==height)&&(srcheight==dstheight)&&(srcwidth==srcheight))){

        //Make the surfaces to hold the source and destinations.
        SDL_Surface *srcsurface = SDL_CreateRGBSurface(0, srcwidth, srcheight, 32, CHANNEL_MASKS);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, srcsurface->pixels);

        //Fill the dst surface with the current pixels of the destination texture.
        glBindTexture(GL_TEXTURE_2D, dstName);

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dstwidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dstheight);

        SDL_Surface *dstsurface = SDL_CreateRGBSurface(0, dstwidth, dstheight, 32, CHANNEL_MASKS);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstsurface->pixels);

        //Copy the pixels needed for the new sub data.

        SDL_Rect srcrect = {srcX, srcY, width, height};
        SDL_Rect dstrect = {dstX, dstY, width, height};

        SDL_BlitSurface(srcsurface, &srcrect, dstsurface, &dstrect);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dstwidth, dstheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstsurface->pixels);

        SDL_FreeSurface(dstsurface);
        SDL_FreeSurface(srcsurface);
    }
    else{
        void * pixels = malloc(width*height);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, dstName);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        free(pixels);
    }
}
#else

void APIENTRY TS_CopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel,
	    GLint srcX, GLint srcY, GLint srcZ,
	    GLuint dstName, GLenum dstTarget, GLint dstLevel,
	    GLint dstX, GLint dstY, GLint dstZ,
	    GLsizei width, GLsizei height, GLsizei depth){

	void * pixels = malloc(width*height);

    int srcwidth, srcheight, dstwidth, dstheight;
    glBindTexture(GL_TEXTURE_2D, dstName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dstwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dstheight);
    glBindTexture(GL_TEXTURE_2D, srcName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &srcwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &srcheight);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, dstName);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    free(pixels);

}
#endif
