#ifndef SDL_GL_MAIN_HEAD
#define SDL_GL_MAIN_HEAD

#ifndef APIENTRY
#define APIENTRY
#endif

#define PLUGINNAME "SDL_GL"
#include "../common/plugin.h"
#ifdef _WIN32
#include "../../SDL/SDL_opengl.h"
#else
#include <SDL/SDL_opengl.h>
#endif
#include "../common/graphic_algorithm.h"
#include "../common/graphic_common.h"
#include "../../configmanager/opengame.h"
#include <cmath>

#ifdef _WIN32
	#define SDL_GL_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SDL_GL_EXPORT extern "C"
#endif

#define NUMFUNCS 20
#define NUMVARS  0

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//this is not possible on proper platforms
//Except MIPS?
//You got this running on MIPS, I wanna know.
#else

    #define Frmask 0x000000ff
	#define Fgmask 0x0000ff00
	#define Fbmask 0x00ff0000
	#define Famask 0xff000000
#endif

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS

//Deprecated.
#define SDL_VIDEO_FLAGS SDL_HWSURFACE|SDL_GL_DOUBLEBUFFER|SDL_OPENGL

#define BPP 4
#define DEPTH 32

//TODO: BIG UGLY HACK FOR SDL2 MIGRATION:
#define SDL_SRCALPHA 0


#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

typedef const char * shaderSource;
typedef GLenum shaderHandle;
typedef GLenum programHandle;

struct TS_GLVideoData{
    bool NVCopyTexture;
    bool ARBCopyTexture;
    bool NVPathRendering;
    bool EXTSecondaryColor;
    bool ARBPointSprite;
};

struct TS_Shader{
    programHandle program;
    shaderHandle vert_shader;
    shaderHandle frag_shader;
    shaderSource vert_source;
    shaderSource frag_source;
};

TS_GLVideoData *GetGLVideoData(void);


extern void (APIENTRY * glGenBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*);
extern void (APIENTRY * glBindBuffer)(GLenum,  GLuint);
extern void (APIENTRY * glBindVertexArray)(GLuint);
extern void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
extern void (APIENTRY * glEnableVertexAttribArray)(GLint);
extern void (APIENTRY * glDisableVertexAttribArray)(GLint);
extern void (APIENTRY * glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
extern void (APIENTRY * glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid*);
//extern void (APIENTRY * CopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);
//extern void (APIENTRY * CopyImageSubDataNV)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);



extern const GLubyte primitiveInd[];

extern SDL_Rect cmpltscreen;

extern SDL_Event event;
extern SDL_Window *screen;

#ifdef _WIN32
	extern "C" {
#endif

SDL_GL_EXPORT void            CCALL Close(void);
SDL_GL_EXPORT initFunction    CCALL Init(void);
SDL_GL_EXPORT int             CCALL GetNumFunctions(void);
SDL_GL_EXPORT functionArray   CCALL GetFunctions(void);
SDL_GL_EXPORT nameArray       CCALL GetFunctionNames(void);
SDL_GL_EXPORT int             CCALL GetNumVariables(void);
SDL_GL_EXPORT v8FunctionArray CCALL GetVariables(void);
SDL_GL_EXPORT nameArray       CCALL GetVariableNames(void);

SDL_GL_EXPORT void *	      CCALL TS_SDL_GL_MakeV8SurfaceHandleFromPixels(int w, int h, void *pixels);

#ifdef _WIN32
	}
#endif


#endif