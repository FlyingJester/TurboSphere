#ifndef TS_SDL_GL_SHADER_HEAD
#define TS_SDL_GL_SHADER_HEAD
#include "main.h"

typedef GLuint TS_Shader;
typedef GLuint TS_ShaderProg;

extern GLint CurrentColorAttrib;
extern GLint CurrentVertexAttrib;
extern GLint CurrentTexcoordAttrib;

struct TS_ScriptShader {
    GLuint name;
    const char *source;
    int used_in_prog;
    GLenum type;
};

struct TS_ShaderS {
    GLuint name;
    TS_ScriptShader* fragment;
    TS_ScriptShader* vertex;
    GLuint vertexAttrib;
    GLuint colorAttrib;
    GLuint texcoordAttrib;
};

v8Function CreateFragShader(V8ARGS);
v8Function CreateVertShader(V8ARGS);
v8Function CreateProgram(V8ARGS);
v8Function LoadProgram(V8ARGS);

v8Function UseProgram(V8ARGS);
v8Function UseCompositeProgram(V8ARGS);

v8Function GetSystemProgram(V8ARGS);
v8Function GetSystemCompositeProgram(V8ARGS);

void TS_ScriptShaderFinalizer(V8FINALIZERARGS(TS_ScriptShader));
void TS_ScriptShaderProgramFinalizer(V8FINALIZERARGS(TS_ShaderS));

TS_Shader TS_LoadSystemShader(const char *file);
TS_Shader TS_LoadShader(const char *file, GLuint *frag = NULL, GLuint *vert = NULL);
TS_ShaderProg TS_CreateProgram(TS_Shader frag, TS_Shader vert);
TS_Shader TS_CreateShader(const char *text, GLenum type, char **error_text=NULL);

#endif
