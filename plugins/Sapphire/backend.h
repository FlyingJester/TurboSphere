#ifndef SAPPHIRE_BACKEND_HEAD
#define SAPPHIRE_BACKEND_HEAD
#include <cstdint>

#ifndef OPENGLES
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles.h>
#endif

extern float whitef[16];
extern float tex[8];

GLint GetColorAttrib(void);
GLint GetTexCoordAttrib(void);
GLint GetVertexAttrib(void);

typedef uint32_t RGBA;

#endif
