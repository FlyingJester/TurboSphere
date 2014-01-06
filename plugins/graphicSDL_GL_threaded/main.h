#ifndef SDL_GL_MAIN_HEAD
#define SDL_GL_MAIN_HEAD

enum buffernames {VertexB = 0, ColorB, TexcoordB};

#define BPP 4
#define DEPTH 32

//TODO: BIG UGLY HACK FOR SDL2 MIGRATION:
#define SDL_SRCALPHA 0

    #define Frmask 0x000000ff
	#define Fgmask 0x0000ff00
	#define Fbmask 0x00ff0000
	#define Famask 0xff000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

#ifndef SDL_GL_EXTERNAL

#ifndef APIENTRY
#define APIENTRY
#endif

#define PLUGINNAME "SDL_GL"
#define BRACKNAME  "[SDL_GL]"
#include "../common/plugin.h"

#include "../../SDL2/SDL_opengl.h"

#include "../common/graphic_algorithm.h"
#include "../common/graphic_common.h"
#include "../../configmanager/opengame.h"
#include <cmath>

#ifdef _WIN32
	#define SDL_GL_EXPORT __declspec(dllexport)

    #define CCALL _cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SDL_GL_EXPORT extern "C"
#endif

#define COLOR_ID ((PluginID<<8)|(0x01u))
#define SURFACE_ID ((PluginID<<8)|(0x02u))
#define IMAGE_ID ((PluginID<<8)|(0x03u))
#define SHADER_ID ((PluginID<<8)|(0x04u))
#define SHADERPROGRAM_ID ((PluginID<<8)|(0x05u))

#include "shader.h"

extern unsigned int ImageID;
extern unsigned int SurfaceID;
extern unsigned int ColorID;
extern unsigned int ShaderID;
extern unsigned int ShaderProgramID;

extern GLuint TS_DefaultShader;
extern GLuint TS_CurrentShader;
extern GLuint TS_DefaultCompositeShader;
extern GLuint TS_CurrentCompositeShader;

#define NUMFUNCS 28
#define NUMVARS  0

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)

#define SDL2_VIDEO_FLAGS SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS

//Deprecated.
#define SDL_VIDEO_FLAGS SDL_HWSURFACE|SDL_GL_DOUBLEBUFFER|SDL_OPENGL


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

TS_GLVideoData *GetGLVideoData(void);

extern GLuint TS_EmptyTexture;

extern void (APIENTRY * glGenBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glBindBuffer)(GLenum,  GLuint);
extern void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
extern void (APIENTRY * glBufferSubData)(GLenum,  GLintptr, GLsizeiptr, const GLvoid *);
extern void (APIENTRY * glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
extern void (APIENTRY * glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid*);
extern void (APIENTRY * glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);
extern void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteVertexArrays)(GLsizei, GLuint*);
extern GLenum (APIENTRY * glCreateShader)(GLenum);
extern void (APIENTRY * glDeleteShader)(GLenum);
extern void (APIENTRY * glShaderSource)(GLenum, GLint, const GLchar **, const GLint *);
extern void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*);
extern void (APIENTRY * glCompileShader)(GLenum);
extern GLenum (APIENTRY * glCreateProgram)(void);
extern void (APIENTRY * glUseProgram)(GLenum);
extern void (APIENTRY * glAttachShader)(GLenum, GLenum);
extern void (APIENTRY * glLinkProgram)(GLenum);
extern void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*);
extern GLboolean (APIENTRY * glIsShader)(GLuint);
extern void (APIENTRY * glGetShaderInfoLog)(GLuint,  GLsizei,  GLsizei *,  GLchar *);
extern void (APIENTRY * glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
extern void (APIENTRY * glDeleteProgram)(GLuint);
extern GLint (APIENTRY *glGetUniformLocation)(GLuint,  const GLchar *);
extern void (APIENTRY * glProgramUniform4fv)(GLuint,  GLint,  GLsizei,  const GLfloat *);
extern void (APIENTRY * glProgramUniform2fv)(GLuint,  GLint,  GLsizei,  const GLfloat *);
extern void (APIENTRY * glProgramUniform2iv)(GLuint,  GLint,  GLsizei,  const GLint *);
extern void (APIENTRY * glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
extern void (APIENTRY * glEnableVertexAttribArray)(GLuint);
extern void (APIENTRY * glDisableVertexAttribArray)(GLuint);
extern void (APIENTRY * glGenFramebuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteFramebuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glBindFramebuffer)(GLenum, GLuint);
extern void (APIENTRY * glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);
extern void (APIENTRY * glBindVertexArray)(GLuint);
extern void (APIENTRY * glBindAttribLocation)(GLuint program, GLuint index, const GLchar *name);
extern GLint (APIENTRY * glGetAttribLocation )(GLuint program, const GLchar *name);
extern GLboolean (APIENTRY * glIsProgram)(GLuint program);
extern GLint (APIENTRY * glGetUniformLocation)(GLuint program, const GLchar *name);

EXTERN_OBJECT_TEMPLATES(ScriptShader);
EXTERN_OBJECT_TEMPLATES(ScriptShaderProgram);

extern int PluginID;

extern const GLubyte primitiveInd[];

extern SDL_Event event;
extern SDL_Window *screen;

#ifdef _WIN32
	extern "C" {
#endif
SDL_GL_EXPORT void            CCALL GetPluginInfo(TS_PluginInfo *info);

SDL_GL_EXPORT void            CCALL Close(void);
SDL_GL_EXPORT initFunction    CCALL Init(int);
SDL_GL_EXPORT int             CCALL GetNumFunctions(void);
SDL_GL_EXPORT functionArray   CCALL GetFunctions(void);
SDL_GL_EXPORT nameArray       CCALL GetFunctionNames(void);
SDL_GL_EXPORT int             CCALL GetNumVariables(void);
SDL_GL_EXPORT v8FunctionArray CCALL GetVariables(void);
SDL_GL_EXPORT nameArray       CCALL GetVariableNames(void);
//SDL_GL_EXPORT extern void            (* FlipScreen)(void);

SDL_GL_EXPORT void CCALL FlagForScreenshot(void);

#ifdef _WIN32
	}
#endif

extern "C" SDL_GL_EXPORT v8Function CCALL TS_SDL_GL_MakeV8SurfaceHandleFromPixels(int w, int h, void *pixels);
extern "C" SDL_GL_EXPORT v8Function CCALL TS_SDL_GL_MakeV8ImageHandleFromGLTexture(int w, int h, GLuint tex);
extern "C" SDL_GL_EXPORT v8Function CCALL TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoord(int w, int h, float coords[8], GLuint tex);
extern "C" SDL_GL_EXPORT v8Function CCALL TS_SDL_GL_WrapTS_Color(TS_Color *c);
extern "C" SDL_GL_EXPORT uint32_t * CCALL TS_SDL_GL_GetSurfaceID(void);
extern "C" SDL_GL_EXPORT uint32_t * CCALL TS_SDL_GL_GetImageID(void);
class TS_Image;
extern "C" SDL_GL_EXPORT unsigned int CCALL TS_SDL_GL_GetTextureFromImage(TS_Image *im);
extern "C" SDL_GL_EXPORT void CCALL ResetOrtho(void);
extern "C" SDL_GL_EXPORT GLuint CCALL TS_SDL_GL_GetCurrentShader(void);
extern "C" SDL_GL_EXPORT GLuint CCALL TS_SDL_GL_GetDefaultShader(void);

#endif

#endif
