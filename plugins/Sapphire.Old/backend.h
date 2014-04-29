#ifndef SAPPHIRE_BACKEND_HEAD
#define SAPPHIRE_BACKEND_HEAD
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
    #define START_STRUCT(X) struct X { //
    #define END_STRUCT(X) } //
#else
    #define START_STRUCT(X) typedef struct { //
    #define END_STRUCT(X) } X //
#endif

/////
// For FJ-GL control
//
// This should never exceed 0xFF.

#define MAX_STACK_STEAL 0x80

#include <SDL2/SDL.h>

#ifndef OPENGL_ES
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles.h>
#endif

#ifndef PLUGINNAME
#define PLUGINNAME "Sapphire"
#endif

typedef uint32_t RGBA;

#ifdef __cplusplus
extern "C"{
#endif

extern float SPI;

extern unsigned int ShaderID;
extern unsigned int ShaderProgramID;

extern GLuint TS_DefaultShader;
extern GLuint TS_CurrentShader;
extern GLuint TS_DefaultCompositeShader;
extern GLuint TS_CurrentCompositeShader;

extern GLfloat stexcoords[];
extern GLfloat ztexcoords[];

extern float whitef[16];
extern GLuint ZeroTexCoordBuffer;
extern GLuint SeriousCoordBuffer;
extern float tex[8];

extern GLuint EmptyTexture;

extern SDL_Window *screen;

GLint GetColorAttrib(void);
GLint GetTexCoordAttrib(void);
GLint GetVertexAttrib(void);

void BackendInit(bool threaded, bool composite);
void BackendClose(void);

extern RGBA *ScreenCopy;

#ifdef __cplusplus
}
#endif


#endif
