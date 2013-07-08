#ifndef MAPENGINE_ME_HEAD
#define MAPENGINE_ME_HEAD
#define PLUGINNAME "mapengineGL"
#define BRACKNAME "[mapengineGL]"
#include"../common/plugin.h"
#ifdef _WIN32
#include "../../SDL/SDL_opengl.h"
#else
#include <SDL/SDL_opengl.h>
#endif
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"

#ifndef APIENTRY
#define APIENTRY
#endif

#ifdef _WIN32
    #ifdef MAPENGINE_INTERNAL
	#define MAPENGINE_EXPORT __declspec(dllexport)
	#else
	#define MAPENGINE_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

    #define STRDUP _strdup

#else
    #define CCALL
    #define MAPENGINE_EXPORT extern "C"

    #define STRDUP strdup
#endif

typedef GLuint TS_Texture;

extern GLuint defaultShaderProgram;

EXTERN_OBJECT_TEMPLATES(Map);

extern void (APIENTRY * glGenBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*);
extern void (APIENTRY * glBindBuffer)(GLenum,  GLuint);
extern void (APIENTRY * glBindVertexArray)(GLuint);
extern void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
extern void (APIENTRY * glEnableVertexAttribArray)(GLint);
extern void (APIENTRY * glDisableVertexAttribArray)(GLint);
extern void (APIENTRY * glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);

#ifdef MAPENGINE_INTERNAL

#define NUMFUNCS 8
#define NUMVARS  0

#ifdef _WIN32
	extern "C" {
#endif
MAPENGINE_EXPORT void          CCALL Close(void);
MAPENGINE_EXPORT initFunction  CCALL Init(void);
MAPENGINE_EXPORT int           CCALL GetNumFunctions(void);
MAPENGINE_EXPORT functionArray CCALL GetFunctions(void);
MAPENGINE_EXPORT nameArray     CCALL GetFunctionNames(void);
MAPENGINE_EXPORT int           CCALL GetNumVariables(void);
MAPENGINE_EXPORT void **       CCALL GetVariables(void);
MAPENGINE_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif

#endif
