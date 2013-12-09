#ifndef TS_SDL_GL_SHADER_HEAD
#define TS_SDL_GL_SHADER_HEAD
#include "main.h"

typedef GLuint TS_Shader;
typedef GLuint TS_ShaderProg;

TS_Shader TS_LoadSystemShader(const char *file);
TS_Shader TS_LoadShader(const char *file);
TS_ShaderProg TS_CreateProgram(TS_Shader frag, TS_Shader vert);
TS_Shader TS_CreateShader(const char *text, GLenum type);

#endif
