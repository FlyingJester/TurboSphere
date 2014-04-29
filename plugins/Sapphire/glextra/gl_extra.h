#ifndef FJGL_GLEXTRA_HEAD
#define FJGL_GLEXTRA_HEAD

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <Wingdi.h>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

typedef size_t GLsizeiptr;
typedef int * GLintptr;
typedef char GLchar;

#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_SUBTRACT                  0x800A

#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_DELETE_STATUS                  0x8B80
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ATTACHED_SHADERS               0x8B85
#define GL_SHADER_TYPE                    0x8B4F
#include <GL/gl.h>
#endif

#include <GL/gl.h>

#ifndef GL_ZERO
#define GL_ZERO	0
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void (APIENTRY * glEnableVertexAttribArray)(GLuint);
extern void (APIENTRY * glDisableVertexAttribArray)(GLuint);
extern void (APIENTRY * glGenBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glBindBuffer)(GLenum,  GLuint);
extern void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
extern void (APIENTRY * glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *);
extern void (APIENTRY * glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);
extern GLenum (APIENTRY * glCreateShader)(GLenum);
extern void (APIENTRY * glDeleteShader)(GLenum);
extern void (APIENTRY * glShaderSource)(GLenum, GLint, const GLchar **, const GLint *);
extern void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*);
extern void (APIENTRY * glCompileShader)(GLenum);
extern GLenum (APIENTRY * glCreateProgram)(void);
extern void (APIENTRY * glUseProgram)(GLenum);
extern bool (APIENTRY * glIsProgram)(GLuint);
extern void (APIENTRY * glAttachShader)(GLenum, GLenum);
extern void (APIENTRY * glLinkProgram)(GLenum);
extern void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*);
extern GLboolean (APIENTRY * glIsShader)(GLuint);
extern void (APIENTRY * glGetShaderInfoLog)(GLuint,  GLsizei,  GLsizei *,  GLchar *);
extern void (APIENTRY * glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
extern void (APIENTRY * glDeleteProgram)(GLuint);
extern GLint(APIENTRY * glGetUniformLocation)(GLuint program, const GLchar *name);
extern void (APIENTRY * glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
extern void (APIENTRY * glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum);
extern void (APIENTRY * glGenerateMipmap)(GLenum);
extern void (APIENTRY * glGenFramebuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteFramebuffers)(GLsizei, GLuint*);
extern void (APIENTRY * glBindFramebuffer)(GLenum, GLuint);
extern void (APIENTRY * glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);

extern void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*);
extern void (APIENTRY * glDeleteVertexArrays)(GLsizei, GLuint*);
extern void (APIENTRY * glBindVertexArray)(GLuint);
extern GLint (APIENTRY * glGetAttribLocation)(GLuint, const GLchar *);
extern GLint (APIENTRY * glVertexAttribPointer)(GLuint name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *data);

#ifdef _WIN32
extern void (APIENTRY * glBlendEquation)(GLenum);
#endif

void LoadGLFunctions(void);

#ifdef __cplusplus
}
#endif
#endif
