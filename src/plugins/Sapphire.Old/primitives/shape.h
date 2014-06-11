#ifndef SAPPHIRE_SHAPE_HEAD
#define SAPPHIRE_SHAPE_HEAD

#include <SDL2/SDL.h>

#ifdef OPENGL_ES
#include <SDL2/SDL_opengles.h>
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif


#define VERTEX_B 0
#define COLORS_B 1
#define TEXCRD_B 2

#ifdef __cplusplus
struct TS_Shape
#else
typedef struct
#endif
{
    GLuint texture;
    GLuint VertexArray;
    GLuint Buffers[3];
    GLuint NumVertices;
}
#ifndef __cplusplus
TS_Shape
#endif
;

#ifdef __cplusplus
extern "C" {
#endif
void DrawShape(TS_Shape*);
void ShapeInit(void);
void ShapeClose(void);
TS_Shape* CreateShape(size_t length, float *vertices, float *texcoords, float *colors, GLuint texture);

#ifdef __cplusplus
}
#endif


#endif
