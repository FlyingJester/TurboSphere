#ifndef SDL_GL_COLOR_HEAD
#define SDL_GL_COLOR_HEAD

EXTERN_OBJECT_TEMPLATES(Color);

extern const uint32_t ColorGLMagic;

void GLColor(TS_Color *color);

void ColorInit(void);
void ColorClose(void);

v8Function CreateColor(V8ARGS);
void TS_ColorFinalizer(V8FINALIZERARGS);

v8Function GetRed(V8GETTERARGS);
void       SetRed(V8SETTERARGS);

v8Function GetGreen(V8GETTERARGS);
void       SetGreen(V8SETTERARGS);

v8Function GetBlue(V8GETTERARGS);
void       SetBlue(V8SETTERARGS);

v8Function GetAlpha(V8GETTERARGS);
void       SetAlpha(V8SETTERARGS);

#endif
